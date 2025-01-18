#pragma once
#include "ARequestHandler.hpp"
#include "defines.hpp"

namespace webkernel
{
class RequestHandlerManager
{
public:
    static RequestHandlerManager& get_instance();

    webshell::Response handle_request(int fd,
                                      EventProcessingState& state,
                                      webshell::Request& request);

private:
    std::map<webshell::RequestMethod, ARequestHandler*> _handlers;

private:
    ~RequestHandlerManager();
    RequestHandlerManager();
    RequestHandlerManager(const RequestHandlerManager&);
    RequestHandlerManager& operator=(const RequestHandlerManager&);
};

} // namespace webkernel
