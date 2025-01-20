#include "RequestHandlerManager.hpp"
#include "DeleteHandler.hpp"
#include "GetHandler.hpp"
#include "HttpException.hpp"
#include "PostHandler.hpp"
#include "defines.hpp"

namespace webkernel
{

RequestHandlerManager::RequestHandlerManager()
{
    _handlers[webshell::GET] = new GetHandler();
    _handlers[webshell::POST] = new PostHandler();
    _handlers[webshell::DELETE] = new DeleteHandler();
}

RequestHandlerManager::~RequestHandlerManager()
{
    for (std::map<webshell::RequestMethod, ARequestHandler*>::iterator it =
             _handlers.begin();
         it != _handlers.end();
         ++it) {
        delete it->second;
    }
}

RequestHandlerManager& RequestHandlerManager::get_instance()
{
    static RequestHandlerManager instance;
    return instance;
}

webshell::Response
RequestHandlerManager::handle_request(int fd,
                                      EventProcessingState& state,
                                      webshell::Request& request)
{
    std::map<webshell::RequestMethod, ARequestHandler*>::iterator it =
        _handlers.find(request.method());
    if (it != _handlers.end()) {
        return it->second->handle(fd, state, request);
    }
    throw utils::HttpException(webshell::NOT_IMPLEMENTED,
                               "Method not implemented");
}

} // namespace webkernel
