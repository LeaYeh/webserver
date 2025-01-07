#include "GetHandler.hpp"
#include "ARequestHandler.hpp"
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

GetHandler::GetHandler() {}

GetHandler::~GetHandler() {}

webshell::Response GetHandler::handle(int fd,
                                      EventProcessingState& state,
                                      webshell::Request& request)
{
    if (state == INITIAL) {
        _preProcess(request);
        _update_status(state, PROCESSING, true);
    }
    if (!request.config().redirect.empty()) {
        return (webshell::ResponseBuilder::redirect(webshell::MOVED_PERMANENTLY,
                                                    request.config().redirect));
    }

    std::string content;
    try {
        content = _process(fd, state, request);
        _postProcess(request, _target_path, content);
    }
    catch (utils::HttpException& e) {
        _handle_exception(e, e.statusCode(), webshell::TEXT_HTML);
    }
    catch (std::exception& e) {
        _handle_exception(e);
    }
    // prepare headers in post process and encode the content if needed e.g.
    // gzip
    return (webshell::ResponseBuilder::ok(webshell::OK,
                                          _response_headers,
                                          content,
                                          state & HANDLE_OTHERS_CHUNKED));
}

void GetHandler::_preProcess(const webshell::Request& request)
{
    ARequestHandler::_preProcess(request);
    const webconfig::RequestConfig& config = request.config();

    _target_path = _get_resource_path(config, request.uri().path);
    if (_target_path.find(config.root) == std::string::npos) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden out of root");
    }
}

std::string GetHandler::_process(int fd,
                                 EventProcessingState& state,
                                 webshell::Request& request)
{
    std::string content;
    const webconfig::RequestConfig& config = request.config();

    if (access(_target_path.c_str(), R_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden no read permission on file: "
                                       + _target_path);
    }
    // if (!config.redirect.empty())
    //     throw utils::HttpException(webshell::MOVED_PERMANENTLY,
    //                                "Redirect to: " + config.redirect);
    if (utils::isDirectory(_target_path)) {
        if (!config.autoindex) {
            throw utils::HttpException(webshell::FORBIDDEN,
                                       "Forbidden autoindex is disabled");
        }
        // for autoindex the content is small so we can read it all at once, it
        // doesn't make sense to chunk it
        _handle_autoindex(state, request.uri().path, config, content);
    }
    else {
        if (_get_respones_encoding(request) & webkernel::CHUNKED) {
            _handle_chunked(fd, state, _target_path, content);
        }
        else {
            _handle_standard(state, _target_path, content);
        }
    }

    return (content);
}

void GetHandler::_postProcess(const webshell::Request& request,
                              const std::string& target_path,
                              const std::string& content)
{
    if (utils::isDirectory(target_path)) {
        _response_headers["content-type"] = "text/html";
    }
    else {
        _response_headers["content-type"] = _getMimeType(target_path);
    }
    if (!utils::isDirectory(target_path)
        && (_get_respones_encoding(request) & webkernel::CHUNKED)) {
        int encoding = _get_respones_encoding(request);
        std::string tmp = _get_encoding_string(encoding);
        _response_headers["transfer-encoding"] = tmp;
    }
    else {
        _response_headers["content-length"] = utils::toString(content.size());
    }
}

void GetHandler::_handle_standard(EventProcessingState& state,
                                  const std::string& target_path,
                                  std::string& content) const
{
    weblog::Logger::log(weblog::DEBUG, "Handle standard: " + target_path);
    std::ifstream file(target_path.c_str(), std::ios::binary);

    if (!file.is_open()) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open file failed");
    }
    content = std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    _update_status(state, COMPELETED, true);
}

void GetHandler::_handle_chunked(int fd,
                                 EventProcessingState& state,
                                 const std::string& target_path,
                                 std::string& content)
{
    weblog::Logger::log(weblog::DEBUG, "Handle chunked: " + target_path);
    std::ifstream file(target_path.c_str(), std::ios::binary);

    if (!file.is_open()) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open file failed");
    }
    _update_status(state, HANDLE_CHUNKED, true);
    std::streampos& file_offset = _chunked_file_records[fd];
    if (file_offset == 0) {
        _update_status(state, HANDLE_FIRST_CHUNKED);
    }
    else {
        _update_status(state, HANDLE_OTHERS_CHUNKED);
    }
    file.seekg(file_offset);
    content.resize(CHUNKED_SIZE);
    file.read(&content[0], CHUNKED_SIZE);
    std::streamsize bytes_read = file.gcount();
    content.resize(bytes_read);
    content = _chunked_codec.encode(content);
    if (file.eof()) {
        _chunked_file_records.erase(fd);
        content += "0\r\n\r\n";
        _update_status(state, COMPELETED, true);
    }
    else if (file.tellg() == -1) {
        _chunked_file_records.erase(fd);
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "file.tellg() failed");
    }
    else {
        file_offset = file.tellg();
    }
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

    if ((dir = opendir(target_path.c_str())) != NULL) {
        std::string object_path;
        while ((ent = readdir(dir)) != NULL) {
            // object_path = request_path + std::string(ent->d_name);
            // if (utils::isFile(target_path + std::string(ent->d_name)))
            object_path = request_path + "/" + std::string(ent->d_name);
            list_items +=
                "<tr><td><a href=\"" + object_path + "\">"
                + std::string(ent->d_name) + "</a></td><td>"
                + get_object_type(target_path + std::string(ent->d_name))
                + "</td><td>"
                + get_object_size(target_path + std::string(ent->d_name))
                + "</td><td>"
                + get_object_mtime(target_path + std::string(ent->d_name))
                + "</td></tr>";
        }
        closedir(dir);
    }
    else {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open directory failed for autoindex: "
                                       + target_path);
    }
    try {
        _template_engine.loadTemplate(config.autoindex_page);
        _template_engine.setVariable("TITLE", "Index of " + request_path);
        _template_engine.setVariable("LIST_ITEMS", list_items);
        content = _template_engine.render();
        _template_engine.reset();
        _update_status(state, COMPELETED, true);
    }
    catch (const std::exception& e) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Template engine failed");
    }
}

} // namespace webkernel
