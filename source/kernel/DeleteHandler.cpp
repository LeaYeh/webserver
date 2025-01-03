#include "DeleteHandler.hpp"
#include "ARequestHandler.hpp"
#include "HttpException.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include <cstdio>

namespace webkernel
{

webshell::Response DeleteHandler::handle(int fd,
                                         EventProcessingState& state,
                                         webshell::Request& request)
{
    if (state == INITIAL) {
        _preProcess(request);
        _update_status(state, PROCESSING, true);
    }
    try {
        _process(fd, state, request);
        _postProcess(request, _target_path, "");
    }
    catch (utils::HttpException& e) {
        _handle_exception(e, e.statusCode());
    }
    catch (std::exception& e) {
        _handle_exception(e);
    }
    return webshell::ResponseBuilder::ok(
        webshell::NO_CONTENT, _response_headers, "", false);
}

void DeleteHandler::_preProcess(const webshell::Request& request)
{
    ARequestHandler::_preProcess(request);
    const webconfig::RequestConfig& config = request.config();

    _target_path = config.root + request.uri().path;
}

std::string DeleteHandler::_process(int fd,
                                    EventProcessingState& state,
                                    webshell::Request& request)
{
    (void)fd;
    (void)request;

    if (!utils::isFile(_target_path)) {
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

void DeleteHandler::_postProcess(const webshell::Request& request,
                                 const std::string& target_path,
                                 const std::string& content)
{
    (void)request;
    (void)target_path;
    (void)content;
}

} // namespace webkernel
