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

    webshell::Response handleRequest(int fd, EventProcessingState& state,
                                     const webconfig::RequestConfig& config,
                                     webshell::Request& request);

  private:
    ~RequestHandlerManager();
    RequestHandlerManager();
    RequestHandlerManager(const RequestHandlerManager&);
    RequestHandlerManager& operator=(const RequestHandlerManager&);

    std::map<webshell::RequestMethod, ARequestHandler*> _handlers;
};

} // namespace webkernel
