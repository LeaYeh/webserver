#include "DeleteHandler.hpp"
#include "ARequestHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <cstdio>
#include <new>
#include <unistd.h>

namespace webkernel
{

webshell::Response DeleteHandler::handle(int fd,
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

void DeleteHandler::_handle_session(webshell::Request& request)
{
    (void)request;
    LOG(weblog::INFO, "DeleteHandler: no session handling, skip");
    _update_status(_state, READY_TO_PROCESS, true);
}

webshell::Response DeleteHandler::_handle_request(int fd,
                                         EventProcessingState& state,
                                         webshell::Request& request)
{
    if (state == READY_TO_PROCESS) {
        _pre_process(request);
        _update_status(state, PROCESSING, true);
    }
    try {
        _process(fd, state, request);
        _post_process(request, _target_path, "");
    }
    catch (utils::HttpException& e) {
        _handle_exception(e, e.status_code());
    }
    catch (std::bad_alloc& e) {
        _handle_exception(e);
    }
    return webshell::ResponseBuilder::ok(
        webshell::NO_CONTENT, _response_headers, "", false);
}

void DeleteHandler::_pre_process(const webshell::Request& request)
{
    ARequestHandler::_pre_process(request);
    LOG(weblog::DEBUG, "DeleteHandler: target path: " + _target_path);
}

std::string DeleteHandler::_process(int fd,
                                    EventProcessingState& state,
                                    webshell::Request& request)
{
    (void)fd;
    (void)request;

    if (utils::is_directory(_target_path)) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Cannot delete directory: " + _target_path);
    }
    if (!ARequestHandler::_check_path_permission(_target_path, F_OK)) {
        throw utils::HttpException(webshell::NOT_FOUND,
                                   "File not found: " + _target_path);
    }
    if (!ARequestHandler::_check_path_permission(_target_path, W_OK)) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "No write permission on file: "
                                       + _target_path);
    }
    if (std::remove(_target_path.c_str()) == -1) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Delete file failed: " + _target_path);
    }
    _update_status(state, COMPELETED, true);
    return "";
}

void DeleteHandler::_post_process(const webshell::Request& request,
                                  const std::string& target_path,
                                  const std::string& content)
{
    (void)request;
    (void)target_path;
    (void)content;
    _response_headers.clear();
}

} // namespace webkernel
