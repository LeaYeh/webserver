#include "Kernel.hpp"
#include "Config.hpp"
#include "ConfigServerBlock.hpp"
#include "ConnectionHandler.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "VirtualHostManager.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>

namespace webkernel
{

Kernel::Kernel()
{
    webconfig::Config* config = webconfig::Config::instance();
    if (config->global_block().worker_processes() == 1) {
        weblog::Logger::log(
            weblog::INFO, "Create single reactor and single worker structure");
        _reactor = new Reactor(REACTOR);
        _acceptor = new Acceptor(_reactor);
        _register_listener();
    }
    else {
        weblog::Logger::log(weblog::INFO,
                            "Create multi reactor and multi worker structure");
        _reactor = new Reactor(DISPATCHER);
        _acceptor = new Acceptor(_reactor);
        _register_listener();
        for (unsigned int i = 1; i < config->global_block().worker_processes();
             i++) {
            pid_t pid = fork();

            if (pid < 0) {
                throw std::runtime_error("fork() failed");
            }
            else if (pid == 0) {
                // child process will
                _reactor = new Reactor(WORKER);
                break;
            }
        }
    }
}

Kernel::Kernel(const Kernel& other) :
    _reactor(other._reactor), _acceptor(other._acceptor)
{
    weblog::Logger::log(weblog::DEBUG, "Kernel::Kernel(const Kernel& other)");
}

Kernel& Kernel::operator=(const Kernel& other)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Kernel::operator=(const Kernel& other)");
    if (this != &other) {
        _reactor = other._reactor;
        _acceptor = other._acceptor;
    }
    return (*this);
}

Kernel::~Kernel()
{
    weblog::Logger::log(weblog::DEBUG, "Kernel::~Kernel()");
    if (_reactor) {
        delete _reactor;
    }
    if (_acceptor) {
        delete _acceptor;
    }
}

void Kernel::run()
{
    weblog::Logger::log(weblog::DEBUG, "Kernel::run()");
    _reactor->run();
}

void Kernel::_register_listener(void)
{
    webconfig::Config* config = webconfig::Config::instance();

    for (size_t i = 0; i < config->server_block_list().size(); i++) {
        webconfig::ConfigServerBlock& servConfig =
            config->server_block_list()[i];
        const std::string ipaddr =
            servConfig.listen().first + ":" + servConfig.listen().second;
        VirtualHostManager& vhost_manager =
            _reactor->conn_handler->vhost_manager;

        if (vhost_manager.has_server(ipaddr)) {
            continue;
        }
        vhost_manager.add_server(ipaddr, &servConfig);
        int fd = _create_listen_socket(servConfig.listen().first.c_str(),
                                       servConfig.listen().second.c_str());
        if (listen(fd, SOMAXCONN) < 0) {
            throw std::runtime_error("listen() failed: "
                                     + std::string(strerror(errno)));
        }
        weblog::Logger::log(weblog::INFO, "Listening on " + ipaddr);
        _reactor->register_handler(fd, _acceptor, EPOLLIN);
    }
}

int Kernel::_create_listen_socket(const char* ip, const char* port)
{
    struct addrinfo hints, *res;
    int status;
    int fd = -1;

    std::fill((char*)&hints, (char*)&hints + sizeof(hints), 0);
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    hints.ai_family = AF_UNSPEC;     // works with both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket, non-blocking
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    try {
        if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
            throw std::runtime_error("getaddrinfo() failed: "
                                     + std::string(gai_strerror(status)));
        }
        fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (fd < 0) {
            throw std::runtime_error("socket() failed: "
                                     + std::string(strerror(errno)));
        }
        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
            < 0) {
            throw std::runtime_error("setsockopt() failed: "
                                     + std::string(strerror(errno)));
        }

        if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) {
            throw std::runtime_error("bind() failed: "
                                     + std::string(strerror(errno)));
        }
    }
    catch (const std::exception& e) {
        freeaddrinfo(res);
        if (fd >= 0) {
            utils::safe_close(fd);
        }
        std::cerr << e.what() << std::endl;
        throw e;
    }
    freeaddrinfo(res);
    return (fd);
}

} // namespace webkernel
