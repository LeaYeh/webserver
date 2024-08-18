#include "Acceptor.hpp"
#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <iostream>

namespace webkernel
{

Acceptor::Acceptor() : _reactor(NULL)
{
}

Acceptor::Acceptor(Reactor* reactor) : _reactor(reactor)
{
}

Acceptor::Acceptor(const Acceptor& other)
{
    if (this != &other)
    {
        _reactor = other._reactor;
    }
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
}

// The ConnectionHandler object is created and registered with the Reactor
// the object will be deleted by the Reactor when the connection is closed
void Acceptor::handle_event(int fd, uint32_t events)
{
    if (events & EPOLLIN)
    {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr*)&client_addr, &addr_size);
        weblog::logger.log(weblog::INFO, "Accepted connection on fd: " +
                                             utils::to_string(conn_fd));
        if (conn_fd < 0)
            throw std::runtime_error("accept() failed: " +
                                     std::string(strerror(errno)));
        ConnectionHandler* conn_handler =
            new ConnectionHandler(conn_fd, _reactor);
        _reactor->register_handler(conn_fd, conn_handler, EPOLLIN | EPOLLOUT);
        weblog::logger.log(weblog::DEBUG,
                           "Registered connection handler with fd: " +
                               utils::to_string(conn_fd));
    }
}

} // namespace webkernel
