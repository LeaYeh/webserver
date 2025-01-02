#include "PostHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "RequestConfig.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "configUtils.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <unistd.h>

namespace webkernel
{

PostHandler::PostHandler() {}

PostHandler::~PostHandler()
{
    for (std::map<int, UploadRecord*>::iterator it =
             _upload_record_pool.begin();
         it != _upload_record_pool.end();
         it++) {
        if (it->second) {
            delete it->second;
        }
    }
    _upload_record_pool.clear();
}

webshell::Response PostHandler::handle(int fd,
                                       EventProcessingState& state,
                                       webshell::Request& request)
{
    try {
        if (state == INITIAL) {
            _preProcess(request);
            _update_status(state, PROCESSING);
        }
        _process(fd, state, request);
    }
    catch (utils::HttpException& e) {
        _handle_exception(e, e.statusCode());
    }
    catch (std::exception& e) {
        _handle_exception(e);
    }
    if (state & COMPELETED) {
        return _handle_completed(fd, request);
    }
    _update_status(state, HANDLE_CHUNKED);
    return webshell::Response();
}

webshell::Response
PostHandler::_handle_completed(int fd, const webshell::Request& request)
{
    _postProcess(request,
                 _upload_record_pool[fd]->target_filename(),
                 _upload_record_pool[fd]->serialize());
    webshell::Response response =
        webshell::ResponseBuilder::ok(webshell::OK,
                                      _response_headers,
                                      _upload_record_pool[fd]->serialize(),
                                      false);
    _upload_record_pool.erase(fd);
    return (response);
}

webshell::Response
PostHandler::_handle_resource_created(const webshell::Request& request)
{
    _check_path_permission(_target_path);
    _postProcess(request, _target_path, "");
    return webshell::ResponseBuilder::ok(
        webshell::CREATED, _response_headers, "", false);
}

void PostHandler::_preProcess(const webshell::Request& request)
{
    ARequestHandler::_preProcess(request);
    const webconfig::RequestConfig& config = request.config();

    _target_path = config.root + config.upload_path;
}

std::string PostHandler::_process(int fd,
                                  EventProcessingState& state,
                                  webshell::Request& request)
{
    // TODO: Refactor this logic into request analyzer @leske42
    // if (request.has_header("expect")) {
    //     unsigned int content_length =
    //         utils::stoi(request.get_header("content-length"));
    //     if (content_length > request.config().client_max_body_size) {
    //         throw utils::HttpException(webshell::PAYLOAD_TOO_LARGE,
    //                                    "Data size exceeds
    //                                    client_max_body_size",
    //                                    webshell::TEXT_PLAIN);
    //     }
    //     throw utils::HttpException(
    //         webshell::CONTINUE, "Continue", webshell::TEXT_PLAIN);
    // }
    weblog::Logger::log(weblog::DEBUG,
                        "PostHandler: handle the request with target path: "
                            + _target_path);
    _check_upload_permission(request);
    _check_path_permission(_target_path);
    _init_upload_record(fd, request);

    std::vector<char> content;
    bool is_eof = request.read_chunked_body(content);

    _write_chunked_file(fd, content);
    _upload_record_pool[fd]->update(is_eof);
    if (is_eof && _upload_record_pool[fd]->success()) {
        _update_status(state, COMPELETED, true);
        return (_upload_record_pool[fd]->serialize());
    }
    _update_status(state, HANDLE_CHUNKED);
    return ("");
}

void PostHandler::_postProcess(const webshell::Request& request,
                               const std::string& target_path,
                               const std::string& content)
{
    // TODO: Add upload redirect
    (void)request;

    _response_headers.clear();
    _response_headers["Content-Type"] =
        webshell::contentTypeToString(webshell::APPLICATION_JSON);
    _response_headers["Content-Length"] = utils::toString(content.size());
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
    return (webshell::contentTypeToString(content_type));
}

std::string
PostHandler::_generate_safe_file_path(const webshell::Request& request)
{
    std::string safe_file_path = _target_path;

    if (utils::isDirectory(safe_file_path)) {
        if (access(safe_file_path.c_str(), F_OK) == -1) {
            throw utils::HttpException(webshell::NOT_FOUND,
                                       "Directory not found: " + safe_file_path,
                                       webshell::TEXT_PLAIN);
        }
        if (safe_file_path[safe_file_path.size() - 1] != '/') {
            safe_file_path += "/";
        }
        std::string file_name = _determine_file_name(request);
        if (file_name.empty()) {
            throw utils::HttpException(
                webshell::NOT_FOUND,
                "Can not determine upload file name from request",
                webshell::TEXT_PLAIN);
        }
        safe_file_path += file_name;
    }
    if (access(utils::basefolder(safe_file_path).c_str(), W_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Not found folder: " + safe_file_path,
                                   webshell::TEXT_PLAIN);
    }
    if (access(safe_file_path.c_str(), F_OK) == -1) {
        std::ofstream file(safe_file_path.c_str(),
                           std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "Open file failed: " + safe_file_path,
                                       webshell::TEXT_PLAIN);
        }
        file.close();
    }
    return (safe_file_path);
}

void PostHandler::_write_chunked_file(int fd, const std::vector<char>& content)
{
    if (_upload_record_pool.find(fd) == _upload_record_pool.end()) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "File stream not found",
                                   webshell::TEXT_PLAIN);
    }
    std::ofstream& file_stream = _upload_record_pool[fd]->file_stream;
    size_t remaining = content.size();
    size_t offset = 0;

    if (!file_stream.is_open()) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "File stream is not open",
                                   webshell::TEXT_PLAIN);
    }
    while (remaining > 0) {
        size_t write_size =
            std::min(remaining, (size_t)webkernel::CHUNKED_SIZE);
        file_stream.write(content.data() + offset, write_size);
        if (!file_stream.good()) {
            throw utils::HttpException(
                webshell::INTERNAL_SERVER_ERROR,
                "Write file failed: " + utils::toString(std::strerror(errno)),
                webshell::TEXT_PLAIN);
        }
        offset += write_size;
        remaining -= write_size;
    }
    file_stream.flush();
}

void PostHandler::_update_status(EventProcessingState& state,
                                 EventProcessingState flags,
                                 bool overwrite)
{
    if (overwrite) {
        state = flags;
    }
    else {
        state = static_cast<EventProcessingState>(state | flags);
    }
    weblog::Logger::log(weblog::WARNING,
                        "PostHandler: update status to "
                            + explainEventProcessingState(state));
}

void PostHandler::_check_upload_permission(const webshell::Request& request)
{
    if (!request.config().enable_upload) {
        throw utils::HttpException(
            webshell::FORBIDDEN, "Upload is not allowed", webshell::TEXT_PLAIN);
    }
}

void PostHandler::_check_path_permission(const std::string& path)
{
    if (access(path.c_str(), F_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Upload path is not allowed",
                                   webshell::TEXT_PLAIN);
    }
    if (access(path.c_str(), W_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Upload path is not writable",
                                   webshell::TEXT_PLAIN);
    }
}

void PostHandler::_init_upload_record(int fd, const webshell::Request& request)
{
    if (_upload_record_pool.find(fd) == _upload_record_pool.end()) {
        std::string tmp = request.get_header("content-length");
        unsigned int content_length = utils::stoi(tmp.empty() ? "0" : tmp);
        _upload_record_pool[fd] =
            new UploadRecord(_generate_safe_file_path(request), content_length);
    }
}

void PostHandler::_handle_exception(const std::exception& e,
                                    webshell::StatusCode code)
{
    weblog::Logger::log(weblog::ERROR, e.what());
    throw utils::HttpException(code, e.what(), webshell::TEXT_PLAIN);
}

} // namespace webkernel
