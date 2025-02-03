#include "CgiHandler.hpp"
#include "ConnectionHandler.hpp"
#include "utils.hpp"
#include "HttpException.hpp"
#include "kernelUtils.hpp"
#include "Logger.hpp"

namespace webkernel
{
    CgiHandler::CgiHandler(webshell::Request& request, int client_fd, int read_end)
        : _request(request), _client_fd(client_fd), _read_end(read_end)
    {
    }

    CgiHandler::~CgiHandler()
    {
    }

    void CgiHandler::handle_event(int fd, uint32_t events)
    {
        (void) fd;
        LOG(weblog::DEBUG,
        "ConnectionHandler::handle_event() on fd: " + utils::to_string(fd)
            + " with events: " + explain_epoll_event(events));
        if (events & EPOLLIN)
        {
            //webkernel::ConnectionHandler::prepare_write(_client_fd, );
        }
        else 
        {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: "
                                       + utils::to_string(fd));
        }
    }
} // namespace webkernel