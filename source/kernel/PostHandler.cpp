#include "PostHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include <unistd.h>

namespace webkernel
{

webshell::Response PostHandler::handle(int fd, EventProcessingState& state,
                                       const webconfig::RequestConfig& config,
                                       webshell::Request& request)
{
    std::string content;

    if (state == INITIAL)
    {
        _preProcess(config, request);
        state = PROCESSING;
    }
    weblog::Logger::log(weblog::DEBUG,
                        "PostHandler: handle the request with target path: " +
                            _target_path);
    content = _process(fd, state, config, request);
    _postProcess(config, request, _target_path, content);
    return webshell::Response();
}

std::string PostHandler::_process(int fd, EventProcessingState& state,
                                  const webconfig::RequestConfig& config,
                                  webshell::Request& request)
{
    if (!config.enable_upload)
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden upload is disabled");
    if (access(_target_path.c_str(), F_OK) &&
        access(_target_path.c_str(), W_OK) == -1)
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden no write permission on file: " +
                                       _target_path);
    std::string content;
    if (request.read_chunked_body(content))
        state = COMPELETED;

    _write_chunked_file(_target_path, content);

    return std::string();
}

} // namespace webkernel
