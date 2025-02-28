#include "GetHandler.hpp"
#include "ARequestHandler.hpp"
#include "ConnectionHandler.hpp"
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

// client_fd
webshell::Response GetHandler::handle(int fd,
                                      EventProcessingState& state,
                                      webshell::Request& request)
{
    _handle_session(request);
    return (_handle_request(fd, state, request));
}

void GetHandler::_handle_session(webshell::Request& request)
{
    std::string session_id = request.get_cookie("session_id");

    if (session_id.empty()) {
        return;
    }
    _response_headers["Set-Cookie"] = _format_session_cookie(
        session_id,
        ConnectionHandler::instance()->get_session_data(session_id));
}

webshell::Response GetHandler::_handle_request(int fd,
                                               EventProcessingState& state,
                                               webshell::Request& request)
{
    std::string content;
    try {
        if (state == READY_TO_PROCESS) {
            _pre_process(request);
            _update_status(state, PROCESSING, true);
        }
        if (!request.config().redirect.empty()) {
            return (_handle_redirect(request.config().redirect));
        }
        if (_is_cgi) {
            // the cgi output is handled by the CgiHandler, so nothing could be
            // responded here
            // TODO: refactor the Reson
            _cgi_executor.cgi_exec(request, fd);
            _update_status(state, COMPELETED, true);
            return (webshell::Response());
        }
        content = _process(fd, state, request);
        _post_process(request, _target_path, content);
    }
    catch (utils::HttpException& e) {
        _handle_exception(e, e.status_code(), webshell::TEXT_HTML);
    }
    catch (std::exception& e) {
        _handle_exception(e);
    }
    return (webshell::ResponseBuilder::ok(webshell::OK,
                                          _response_headers,
                                          content,
                                          state & HANDLE_OTHERS_CHUNKED));
}

void GetHandler::_pre_process(const webshell::Request& request)
{
    ARequestHandler::_pre_process(request);
    const webconfig::RequestConfig& config = request.config();

    _target_path = _get_resource_path(config, request.uri().path);
    if (_target_path.find(config.root) == std::string::npos) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden out of root");
    }
    _is_cgi = true;
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
    if (utils::is_directory(_target_path)) {
        if (!config.autoindex) {
            throw utils::HttpException(webshell::FORBIDDEN,
                                       "Forbidden autoindex is disabled");
        }
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

void GetHandler::_post_process(const webshell::Request& request,
                               const std::string& target_path,
                               const std::string& content)
{
    if (utils::is_directory(target_path)) {
        _response_headers["content-type"] = "text/html";
    }
    else {
        _response_headers["content-type"] = _get_mime_type(target_path);
    }
    if (!utils::is_directory(target_path)
        && (_get_respones_encoding(request) & webkernel::CHUNKED)) {
        int encoding = _get_respones_encoding(request);
        _response_headers["transfer-encoding"] = _get_encoding_string(encoding);
    }
    else {
        _response_headers["content-length"] = utils::to_string(content.size());
    }
}

void GetHandler::_handle_standard(EventProcessingState& state,
                                  const std::string& target_path,
                                  std::string& content) const
{
    LOG(weblog::DEBUG, "Handle standard: " + target_path);
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
    LOG(weblog::DEBUG, "Handle chunked: " + target_path);
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
        content += CHUNKED_END_MARKER;
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

    LOG(weblog::DEBUG, "Handle autoindex: " + request_path);

    if ((dir = opendir(target_path.c_str())) != NULL) {
        std::string object_path;
        while ((ent = readdir(dir)) != NULL) {
            object_path = utils::join(request_path, std::string(ent->d_name));
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
        _template_engine.load_template(config.autoindex_page);
        _template_engine.set_variable("TITLE", "Index of " + request_path);
        _template_engine.set_variable("LIST_ITEMS", list_items);
        content = _template_engine.render();
        _template_engine.reset();
        _update_status(state, COMPELETED, true);
    }
    catch (const std::exception& e) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Template engine failed");
    }
}

webshell::Response GetHandler::_handle_redirect(const std::string& path) const
{
    return (
        webshell::ResponseBuilder::redirect(webshell::MOVED_PERMANENTLY, path));
}

} // namespace webkernel
