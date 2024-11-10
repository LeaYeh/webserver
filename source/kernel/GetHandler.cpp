#include "GetHandler.hpp"

namespace webkernel
{
webshell::Response GetHandler::handle(const webconfig::RequestConfig& config,
                                      const webshell::Request& request) const
{
    _preProcess(config, request);
    
    return (webshell::Response());
}

std::map<std::string, std::string> GetHandler::responseHeaders(void) const
{
    return (_response_headers);
}

} // namespace webkernel
