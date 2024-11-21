#include "RequestHandlerManager.hpp"
#include "GetHandler.hpp"
#include "HttpException.hpp"
#include "defines.hpp"

namespace webkernel
{

RequestHandlerManager::RequestHandlerManager()
{
    _handlers[webshell::GET] = new GetHandler();
}

RequestHandlerManager::~RequestHandlerManager()
{
    for (std::map<webshell::RequestMethod, ARequestHandler*>::iterator it =
             _handlers.begin();
         it != _handlers.end(); ++it)
    {
        delete it->second;
    }
}

RequestHandlerManager& RequestHandlerManager::getInstance()
{
    static RequestHandlerManager instance;
    return instance;
}

webshell::Response
RequestHandlerManager::handleRequest(int fd, EventProcessingState& state,
                                     const webconfig::RequestConfig& config,
                                     const webshell::Request& request)
{
    std::map<webshell::RequestMethod, ARequestHandler*>::iterator it =
        _handlers.find(request.method());
    if (it != _handlers.end())
    {
        return it->second->handle(fd, state, config, request);
    }
    throw utils::HttpException(webshell::NOT_IMPLEMENTED,
                               "Method not implemented");
}

} // namespace webkernel
