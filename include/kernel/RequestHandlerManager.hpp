#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"

namespace webkernel
{
class RequestHandlerManager
{
  public:
    static RequestHandlerManager& getInstance();

    webshell::Response handleRequest(webshell::RequestMethod method,
                                     const webconfig::RequestConfig& config,
                                     const webshell::Request& request) const;

  private:
    RequestHandlerManager();
    ~RequestHandlerManager();
    RequestHandlerManager(const RequestHandlerManager&);
    RequestHandlerManager& operator=(const RequestHandlerManager&);

    std::map<webshell::RequestMethod, const ARequestHandler*> _handlers;
};

} // namespace webkernel
