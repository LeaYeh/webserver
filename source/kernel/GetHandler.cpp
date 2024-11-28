#include "GetHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "ResponseBuilder.hpp"
#include "TemplateEngine.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <dirent.h>
#include <string>
#include <sys/types.h>

namespace webkernel
{

webshell::Response GetHandler::handle(int fd, EventProcessingState& state,
                                      const webconfig::RequestConfig& config,
                                      const webshell::Request& request)
{
    if (state == INITIAL)
    {
        _preProcess(config, request);
        state = PROCESSING;
    }
    weblog::Logger::log(weblog::DEBUG, "GetHandler handle: " + _target_path);
    std::string content;

    if (access(_target_path.c_str(), R_OK) == -1)
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden no read permission on file: " +
                                       _target_path);
    if (!config.redirect.empty())
        throw utils::HttpException(webshell::MOVED_PERMANENTLY,
                                   "Redirect to " + config.redirect);
    if (utils::isDirectory(_target_path))
    {
        if (!config.autoindex)
            throw utils::HttpException(webshell::FORBIDDEN,
                                       "Forbidden autoindex is disabled");
        // for autoindex the content is small so we can read it all at once, it
        // doesn't make sense to chunk it
        _handle_autoindex(state, request.uri().path, config, content);
    }
    else
    {
        if (_get_respones_encoding(config, request) & webkernel::CHUNKED)
            _handle_chunked(fd, state, _target_path, content);
        else
            _handle_standard(state, _target_path, content);
    }

    // prepare headers in post process and encode the content if needed e.g.
    // gzip
    _postProcess(config, request, _target_path, content);
    return (webshell::ResponseBuilder::buildResponse(
        webshell::OK, _response_headers, content,
        state & WRITE_OTHERS_CHUNKED));
}

void GetHandler::_handle_standard(EventProcessingState& state,
                                  const std::string& target_path,
                                  std::string& content) const
{
    weblog::Logger::log(weblog::DEBUG, "Handle standard: " + target_path);
    std::ifstream file(target_path.c_str(), std::ios::binary);

    if (!file.is_open())
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open file failed");
    content = std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    state = COMPELETED;
}

void GetHandler::_handle_chunked(int fd, EventProcessingState& state,
                                 const std::string& target_path,
                                 std::string& content)
{
    weblog::Logger::log(weblog::DEBUG, "Handle chunked: " + target_path);
    std::ifstream file(target_path.c_str(), std::ios::binary);

    if (!file.is_open())
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open file failed");
    state = WRITE_CHUNKED;
    std::streampos& file_offset = _chunked_file_records[fd];
    if (file_offset == 0)
        state = static_cast<EventProcessingState>(state | WRITE_FIRST_CHUNKED);
    else
        state = static_cast<EventProcessingState>(state | WRITE_OTHERS_CHUNKED);
    file.seekg(file_offset);
    content.resize(CHUNKED_SIZE);
    file.read(&content[0], CHUNKED_SIZE);
    std::streamsize bytes_read = file.gcount();
    content.resize(bytes_read);
    content = _chunked_codec.encode(content);
    if (file.eof())
    {
        _chunked_file_records.erase(fd);
        content += "0\r\n\r\n";
        state = static_cast<EventProcessingState>(state | COMPELETED);
    }
    else if (file.tellg() == -1)
    {
        _chunked_file_records.erase(fd);
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "file.tellg() failed");
    }
    else
        file_offset = file.tellg();
    file.close();
}

void GetHandler::_handle_autoindex(EventProcessingState& state,
                                   const std::string& request_path,
                                   const webconfig::RequestConfig& config,
                                   std::string& content)
{
    DIR* dir;
    struct dirent* ent;
    std::string list_items;
    std::string target_path = _get_resource_path(config, request_path);

    weblog::Logger::log(weblog::DEBUG, "Handle autoindex: " + request_path);

    if ((dir = opendir(target_path.c_str())) != NULL)
    {
        std::string object_path;
        while ((ent = readdir(dir)) != NULL)
        {
            // object_path = request_path + std::string(ent->d_name);
            // if (utils::isFile(target_path + std::string(ent->d_name)))
            object_path = request_path + "/" + std::string(ent->d_name);
            list_items +=
                "<tr><td><a href=\"" + object_path + "\">" +
                std::string(ent->d_name) + "</a></td><td>" +
                get_object_type(target_path + std::string(ent->d_name)) +
                "</td><td>" +
                get_object_size(target_path + std::string(ent->d_name)) +
                "</td><td>" +
                get_object_mtime(target_path + std::string(ent->d_name)) +
                "</td></tr>";
        }
        closedir(dir);
    }
    else
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open directory failed for autoindex: " +
                                       target_path);
    try
    {
        _template_engine.loadTemplate(config.autoindex_page);
        _template_engine.setVariable("TITLE", "Index of " + request_path);
        _template_engine.setVariable("LIST_ITEMS", list_items);
        content = _template_engine.render();
        _template_engine.reset();
        state = COMPELETED;
    }
    catch (const std::exception& e)
    {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Template engine failed");
    }
}

} // namespace webkernel
