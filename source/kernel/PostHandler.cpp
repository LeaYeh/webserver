#include "PostHandler.hpp"
#include "ARequestHandler.hpp"
#include "CgiExecutor.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "RequestConfig.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "configUtils.hpp"
#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <new>
#include <string>
#include <unistd.h>

namespace webkernel
{

PostHandler::PostHandler() {}

PostHandler::~PostHandler() {}

webshell::Response PostHandler::handle(int fd,
                                      EventProcessingState& state,
                                      webshell::Request& request)
{
    if (state == WAITING_SESSION) {
        _handle_session(request);
    }
    if (state < READY_TO_PROCESS) {
        return (webshell::Response());
    }
    return (_handle_request(fd, state, request));
}

void PostHandler::_handle_session(webshell::Request& request)
{
    std::string session_id = request.get_cookie("session_id");

    if (session_id.empty()) {
        LOG(weblog::WARNING, "POST request without session id, cannot update session date");
    }
    else {
        std::string data = _format_session_cookie(session_id, "TODO: add game state");
        if (!ConnectionHandler::instance()->set_session_data(session_id, data)) {
            throw std::runtime_error(
                "PostHandler: failed to set session data");
        }
        _response_headers["Set-Cookie"] = "session_id=" + session_id;
    }
    _update_status(_state, READY_TO_PROCESS, true);
}

webshell::Response PostHandler::_handle_request(int fd,
                                       EventProcessingState& state,
                                       webshell::Request& request)
{
    try {
        if (state == READY_TO_PROCESS) {
            _pre_process(request);
            _update_status(state, PROCESSING);
        }
        if (request.is_cgi()) {
            std::string temp_file_path = request.read_chunked_body();
            if (temp_file_path.empty()) {
                _update_status(state, HANDLE_CHUNKED);
            }
            else {
                CgiExecutor::instance()->cgi_exec(request, fd);
                _update_status(state, WAITING_CGI, true);
            }
            return (webshell::Response());
        }
        _process(fd, state, request);
    }
    catch (utils::HttpException& e) {
        std::remove(_temp_file_path.c_str());
        _handle_exception(e, e.status_code(), webshell::TEXT_PLAIN);
    }
    catch (std::bad_alloc& e) {
        std::remove(_temp_file_path.c_str());
        _handle_exception(e);
    }
    if (state & COMPELETED) {
        return _handle_completed(request);
    }
    _update_status(state, HANDLE_CHUNKED);
    return webshell::Response();
}

webshell::Response
PostHandler::_handle_completed(const webshell::Request& request)
{
    std::string message;
    webshell::StatusCode status_code;

    if (_already_exist(_upload_file_path)) {
        // TODO: Add append content will be more make sense for the POST method
        if (std::remove(_upload_file_path.c_str()) == -1) {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "Failed to remove existing file");
        }
        message = "File already exists, replace the content";
        status_code = webshell::OK;
    }
    else {
        message = "Upload completed";
        status_code = webshell::CREATED;
    }
    if (std::rename(_temp_file_path.c_str(), _upload_file_path.c_str()) == -1) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Failed to rename temp file");
    }
    _post_process(request, _upload_file_path, message);
    webshell::Response response = webshell::ResponseBuilder::ok(
        status_code, _response_headers, message, false);
    return (response);
}

void PostHandler::_pre_process(const webshell::Request& request)
{
    ARequestHandler::_pre_process(request);

    _upload_file_path = _generate_safe_file_path(request);
}

std::string PostHandler::_process(int fd,
                                  EventProcessingState& state,
                                  webshell::Request& request)
{
    (void)fd;
    LOG(weblog::DEBUG,
        "PostHandler: handle the request with target path: " + _target_path);
    _check_upload_permission(request);
    if (!_check_path_permission(_target_path, W_OK)) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "No write permission on file: "
                                       + _target_path);
    }

    std::string temp_file_path = request.read_chunked_body();
    if (temp_file_path.empty()) {
        _update_status(state, HANDLE_CHUNKED);
        return ("");
    }
    _temp_file_path = temp_file_path;
    _update_status(state, COMPELETED, true);
    return ("");
}

void PostHandler::_post_process(const webshell::Request& request,
                                const std::string& target_path,
                                const std::string& content)
{
    // TODO: Add upload redirect
    (void)request;
    _response_headers.clear();
    _response_headers["Connection"] = "close";
    _response_headers["Content-Type"] =
        webshell::content_type_to_string(webshell::TEXT_PLAIN);
    _response_headers["Content-Length"] = utils::to_string(content.size());
    _response_headers["Location"] = target_path;
}

std::string PostHandler::_determine_file_name(const webshell::Request& request)
{
    std::string file_name;

    if (utils::basename(request.uri().path) != "") {
        file_name = utils::basename(request.uri().path);
    }
    if (request.has_header("x-file-name")) {
        file_name = request.get_header("x-file-name");
    }
    else if (!request.uri().query.empty()) {
        file_name = request.uri().query;
    }

    weblog::Logger::log(weblog::DEBUG, "Upload File name: " + file_name);
    return (file_name);
}

std::string
PostHandler::_determine_content_type(const webshell::Request& request)
{
    webshell::ContentType content_type = webshell::TEXT_PLAIN;

    if (request.has_header("content-type")) {
        content_type = webconfig::string_to_content_type(
            request.get_header("content-type"));
    }
    else if (!request.uri().query.empty()) {
        std::string extension = utils::file_extension(request.uri().query);
        if (extension == "html") {
            content_type = webshell::TEXT_HTML;
        }
        else if (extension == "css") {
            content_type = webshell::TEXT_CSS;
        }
        else if (extension == "jpg" || extension == "jpeg") {
            content_type = webshell::IMAGE_JPEG;
        }
        else if (extension == "png") {
            content_type = webshell::IMAGE_PNG;
        }
        else if (extension == "gif") {
            content_type = webshell::IMAGE_GIF;
        }
        else if (extension == "json") {
            content_type = webshell::APPLICATION_JSON;
        }
    }
    return (webshell::content_type_to_string(content_type));
}

std::string
PostHandler::_generate_safe_file_path(const webshell::Request& request)
{
    std::string safe_file_path = _target_path;

    if (utils::is_directory(safe_file_path)) {
        if (access(safe_file_path.c_str(), F_OK) == -1) {
            throw utils::HttpException(
                webshell::NOT_FOUND, "Directory not found: " + safe_file_path);
        }
        if (safe_file_path[safe_file_path.size() - 1] != '/') {
            safe_file_path += "/";
        }
        std::string file_name = _determine_file_name(request);
        if (file_name.empty()) {
            throw utils::HttpException(
                webshell::NOT_FOUND,
                "Can not determine upload file name from request");
        }
        safe_file_path += file_name;
    }
    if (access(utils::basefolder(safe_file_path).c_str(), W_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Not found folder: " + safe_file_path);
    }
    return (safe_file_path);
}

bool PostHandler::_already_exist(const std::string& file_path)
{
    return (access(file_path.c_str(), F_OK) != -1);
}

void PostHandler::_check_upload_permission(const webshell::Request& request)
{
    if (!request.config().enable_upload) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Upload is not allowed");
    }
}

} // namespace webkernel
