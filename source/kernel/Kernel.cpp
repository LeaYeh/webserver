#include "Kernel.hpp"
#include "ConfigServerBlock.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <typeinfo>

namespace webkernel
{

Kernel::Kernel(webconfig::Config& config) : _config(config)
{
    _init();
}

Kernel::Kernel(const Kernel& other)
    : _config(other._config), _reactor(other._reactor),
      _acceptor(other._acceptor)
{
    weblog::logger.log(weblog::DEBUG, "Kernel::Kernel(const Kernel& other)");
}

Kernel& Kernel::operator=(const Kernel& other)
{
    weblog::logger.log(weblog::DEBUG, "Kernel::operator=(const Kernel& other)");
    if (this != &other)
    {
        _config = other._config;
        _reactor = other._reactor;
        _acceptor = other._acceptor;
    }
    return (*this);
}

Kernel::~Kernel()
{
    weblog::logger.log(weblog::DEBUG, "Kernel::~Kernel()");
    if (_reactor)
        delete _reactor;
}

void Kernel::run()
{
    weblog::logger.log(weblog::DEBUG, "Kernel::run()");
    _reactor->run();
}

void Kernel::_init(void)
{
    if (_config.global_block().worker_processes() == 1)
        _create_single_reactor_single_worker();
    else
        _create_multi_reactor_multi_worker();
}

void Kernel::_init_listener(void)
{
    for (size_t i = 0; i < _config.server_block_list().size(); i++)
    {
        webconfig::ConfigServerBlock config = _config.server_block_list()[i];
        int fd = _create_listen_socket(config.listen().first.c_str(),
                                       config.listen().second.c_str());
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error("listen() failed: " +
                                     std::string(strerror(errno)));
        weblog::logger.log(weblog::INFO, "Listening on " +
                                             config.listen().first + ":" +
                                             config.listen().second);
        _acceptor->setup_server_id(i);
        _reactor->register_handler(fd, _acceptor, EPOLLIN);
        weblog::logger.log(weblog::INFO, "Registered acceptor with fd: " +
                                             utils::to_string(fd));
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

    try
    {
        if ((status = getaddrinfo(ip, port, &hints, &res)) != 0)
            throw std::runtime_error("getaddrinfo() failed: " +
                                     std::string(gai_strerror(status)));
        fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (fd < 0)
            throw std::runtime_error("socket() failed: " +
                                     std::string(strerror(errno)));
        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
            0)
            throw std::runtime_error("setsockopt() failed: " +
                                     std::string(strerror(errno)));

        if (bind(fd, res->ai_addr, res->ai_addrlen) < 0)
            throw std::runtime_error("bind() failed: " +
                                     std::string(strerror(errno)));
    }
    catch (const std::exception& e)
    {
        freeaddrinfo(res);
        if (fd >= 0)
            utils::safe_close(fd);
        std::cerr << e.what() << std::endl;
        throw e;
    }
    freeaddrinfo(res);
    _fd_server_id_map[fd] = -1;
    return (fd);
}

void Kernel::_create_single_reactor_single_worker(void)
{
    weblog::logger.log(weblog::INFO,
                       "Creating single reactor and single worker structure");
    _reactor = new Reactor(REACTOR);
    _acceptor = new Acceptor(_reactor, &_config);
    _init_listener();
}

void Kernel::_create_multi_reactor_multi_worker(void)
{
    weblog::logger.log(weblog::INFO,
                       "Creating multi reactor and multi worker structure");
    _reactor = new Reactor(DISPATCHER);
    _acceptor = new Acceptor(_reactor, &_config);
    _init_listener();
    for (unsigned int i = 1; i < _config.global_block().worker_processes(); i++)
    {
        pid_t pid = fork();

        if (pid < 0)
            throw std::runtime_error("fork() failed");
        else if (pid == 0)
        {
            // child process will
            _reactor = new Reactor(WORKER);
            break;
        }
    }
}

} // namespace webkernel
