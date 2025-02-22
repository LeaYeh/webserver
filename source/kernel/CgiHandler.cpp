#include "CgiHandler.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>

namespace webkernel
{

CgiHandler::CgiHandler(webshell::Request& request, int client_fd) :
    _buffer(""), _client_fd(client_fd)
{
    (void)request;
}

CgiHandler::~CgiHandler() {}

void CgiHandler::handle_event(int fd /*read_end*/, uint32_t events)
{
    LOG(weblog::CRITICAL,
        "CgiHandler::handle_event() on fd: " + utils::to_string(fd)
            + " with events: " + explain_epoll_event(events));

    if (events & EPOLLIN) {
        char buffer[CHUNKED_SIZE];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(fd, buffer, CHUNKED_SIZE);

        std::cout << "bytes_read: " << bytes_read << std::endl;
        if (bytes_read > 0) {
            _buffer.append(buffer, bytes_read);
        }
        else if (bytes_read == 0) {
            webkernel::ConnectionHandler::instance()->prepare_write(_client_fd,
                                                                    _buffer);
            webkernel::Reactor::instance()->remove_handler(fd);
            // TODO: delete this;
        }
        else {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "read() failed: "
                                           + std::string(strerror(errno)));
        }
    }
    else if (events & EPOLLHUP) {
        webkernel::ConnectionHandler::instance()->prepare_write(_client_fd,
                                                                _buffer);
        webkernel::Reactor::instance()->remove_handler(fd);
    }
    else {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: "
                                       + utils::to_string(fd));
    }
}

} // namespace webkernel
