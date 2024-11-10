#include "GetHandler.hpp"

namespace webkernel
{
webshell::Response GetHandler::handle(const webconfig::AConfigParser* config,
                                      const webshell::Request& request) const
{
    (void)config;
    (void)request;
    return (webshell::Response());
}

std::map<std::string, std::string> GetHandler::responseHeaders(void) const
{
    return (_response_headers);
}

bool GetHandler::_preProcess(const webconfig::AConfigParser* config,
                             const webshell::Request& request)
{
    (void)config;
    if (!checkPathFormat(request.uri().path))
    {
        _response_headers["Content-Type"] = "text/html";
        _response_headers["Content-Length"] = "0";
        return (false);
    }
    return (true);
}

bool GetHandler::_postProcess(webconfig::AConfigParser* config,
                              webshell::Request& request)
{
    (void)config;
    (void)request;
    return (true);
}

} // namespace webkernel
