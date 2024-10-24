#include "Acceptor.hpp"
#include "Config.hpp"
#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <sys/epoll.h>

namespace webkernel
{

Acceptor::Acceptor(Reactor* reactor) : _reactor(reactor)
{
}

Acceptor::Acceptor(const Acceptor& other) : _reactor(other._reactor)
{
}

Acceptor& Acceptor::operator=(const Acceptor& other)
{
    if (this != &other)
    {
        _reactor = other._reactor;
    }
    return (*this);
}

Acceptor::~Acceptor()
{
    weblog::Logger::log(weblog::DEBUG, "Acceptor destroyed");
}

// The ConnectionHandler object is created and registered with the Reactor
// the object will be deleted by the Reactor when the connection is closed
// Register the connection handler with the reactor and set it to epoll edge
// triggered mode with Level Triggered (LT) mode as the default
void Acceptor::handleEvent(int fd, uint32_t events)
{
    if (events & EPOLLIN)
    {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr*)&client_addr, &addr_size);

        weblog::Logger::log(weblog::INFO, "Accepted connection on fd: " +
                                              utils::toString(conn_fd));
        if (conn_fd < 0)
            throw std::runtime_error("accept() failed: " +
                                     std::string(strerror(errno)));
        int server_id = _reactor->lookupServerId(fd);
        _reactor->registerHandler(conn_fd, server_id, _reactor->conn_handler,
                                  EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);
        // _reactor->registerHandler(conn_fd, server_id, _reactor->conn_handler,
        // EPOLLIN);
        weblog::Logger::log(weblog::DEBUG,
                            "Registered connection handler with fd: " +
                                utils::toString(conn_fd));
    }
    else
        weblog::Logger::log(weblog::ERROR, "Acceptor got unknown event: " +
                                               explainEpollEvent(events));
}

} // namespace webkernel
