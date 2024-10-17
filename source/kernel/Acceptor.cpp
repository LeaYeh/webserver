#include "Acceptor.hpp"
#include "Config.hpp"
#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <iostream>

namespace webkernel
{

Acceptor::Acceptor() : _reactor(NULL), _config(NULL), _server_id(-1)
{
}

Acceptor::Acceptor(Reactor* reactor, webconfig::Config* config)
    : _reactor(reactor), _config(config), _server_id(-1)
{
}

Acceptor::Acceptor(const Acceptor& other)
    : _reactor(other._reactor), _config(other._config),
      _server_id(other._server_id)
{
}

Acceptor& Acceptor::operator=(const Acceptor& other)
{
    if (this != &other)
    {
        _reactor = other._reactor;
        _config = other._config;
        _server_id = other._server_id;
    }
    return (*this);
}

Acceptor::~Acceptor()
{
    weblog::logger.log(weblog::DEBUG, "Acceptor destroyed with server_id: " +
                                          utils::to_string(_server_id));
}

// The ConnectionHandler object is created and registered with the Reactor
// the object will be deleted by the Reactor when the connection is closed
// Register the connection handler with the reactor and set it to epoll edge
// triggered mode with EPOLLET
void Acceptor::handleEvent(int fd, uint32_t events)
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
            new ConnectionHandler(conn_fd, _reactor, _config, _server_id);
        _reactor->register_handler(conn_fd, conn_handler, EPOLLIN | EPOLLET);
        weblog::logger.log(weblog::DEBUG,
                           "Registered connection handler with fd: " +
                               utils::to_string(conn_fd));
    }
}

void Acceptor::setup_server_id(int server_id)
{
    weblog::logger.log(weblog::DEBUG, "Acceptor setup with server_id: " +
                                          utils::to_string(server_id));
    _server_id = server_id;
}

} // namespace webkernel
