#include "CgiHandler.hpp"
#include "ConnectionHandler.hpp"
#include "utils.hpp"
#include "HttpException.hpp"
#include "kernelUtils.hpp"
#include "Logger.hpp"
#include <sys/types.h>

namespace webkernel
{
    CgiHandler::CgiHandler(webshell::Request& request, int client_fd, int read_end)
        : _request(request), _buffer(""), _client_fd(client_fd), _read_end(read_end)
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
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytes_read = read(_read_end, buffer, sizeof(buffer));
            if (bytes_read > 0)
            {
                _buffer.append(buffer, bytes_read);
            }
            else
            {
                webkernel::ConnectionHandler::instance()->prepare_write(_client_fd, _buffer);
                webkernel::Reactor::instance()->remove_handler(fd);
                //TODO: delete this;
            }
        }
        else 
        {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: "
                                       + utils::to_string(fd));
        }
    }
} // namespace webkernel