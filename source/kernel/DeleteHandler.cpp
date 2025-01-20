#include "DeleteHandler.hpp"
#include "ARequestHandler.hpp"
#include "HttpException.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <cstdio>
#include <unistd.h>

namespace webkernel
{

webshell::Response DeleteHandler::handle(int fd,
                                         EventProcessingState& state,
                                         webshell::Request& request)
{
    if (state == INITIAL) {
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
    catch (std::exception& e) {
        _handle_exception(e);
    }
    return webshell::ResponseBuilder::ok(
        webshell::NO_CONTENT, _response_headers, "", false);
}

void DeleteHandler::_pre_process(const webshell::Request& request)
{
    ARequestHandler::_pre_process(request);
    const webconfig::RequestConfig& config = request.config();

    _target_path = config.root + request.uri().path;
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
