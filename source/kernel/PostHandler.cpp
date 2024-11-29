#include "PostHandler.hpp"
#include "Logger.hpp"

namespace webkernel
{

webshell::Response PostHandler::handle(int fd, EventProcessingState& state,
                                       const webconfig::RequestConfig& config,
                                       const webshell::Request& request)
{
    std::string content;

    if (state == INITIAL)
    {
        _preProcess(config, request);
        state = PROCESSING;
    }
    content = _process(fd, state, config, request);
    _postProcess(config, request, _target_path, content);
    return webshell::Response();
}

std::string PostHandler::_process(int fd, EventProcessingState& state,
                                  const webconfig::RequestConfig& config,
                                  const webshell::Request& request)
{
    (void)fd;
    (void)state;
    (void)config;
    (void)request;
    return std::string();
}

} // namespace webkernel
