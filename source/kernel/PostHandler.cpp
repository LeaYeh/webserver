#include "PostHandler.hpp"
#include "Logger.hpp"

namespace webkernel
{

webshell::Response PostHandler::handle(int fd, EventProcessingState& state,
                                       const webconfig::RequestConfig& config,
                                       const webshell::Request& request)
{
    std::string content;

    switch (state)
    {
    case INITIAL:
        _preProcess(config, request);
        state = PROCESSING;
        break;
    case PROCESSING:
        break;
    case COMPELETED:
        break;
    case WRITE_CHUNKED:
        break;
    default:
        weblog::Logger::log(weblog::ERROR,
                            "Unknown state " + utils::toString(state) +
                                " on fd: " + utils::toString(fd));
        break;
    }
    _postProcess(config, request, _target_path, content);
    return webshell::Response();
}

} // namespace webkernel
