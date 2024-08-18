#include "Reactor.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <cerrno>
#include <cstdio>
#include <unistd.h>

namespace webkernel
{

Reactor::Reactor() : _type(UNDEFINED)
{
    weblog::logger.log(weblog::DEBUG, "Reactor::Reactor()");
    _epoll_fd = -1;
}

Reactor::Reactor(ReactorType type) : _type(type)
{
    weblog::logger.log(weblog::DEBUG, "Reactor::Reactor(ReactorType type)");
    _init_epoll();
}

Reactor::Reactor(const Reactor& other)
    : _type(other._type), _epoll_fd(other._epoll_fd)
{
    weblog::logger.log(weblog::DEBUG, "Reactor::Reactor(const Reactor& other)");
}

Reactor& Reactor::operator=(const Reactor& other)
{
    weblog::logger.log(weblog::DEBUG,
                       "Reactor::operator=(const Reactor& other)");
    if (this != &other)
    {
        _type = other._type;
        _epoll_fd = other._epoll_fd;
    }
    return (*this);
}

Reactor::~Reactor()
{
    weblog::logger.log(weblog::DEBUG, "Reactor::~Reactor()");
    utils::safe_close(_epoll_fd);
    for (std::map<int, IHandler*>::iterator it = _handlers.begin();
         it != _handlers.end(); it++)
    {
        close(it->first);
        delete it->second;
    }
}

void Reactor::run(void)
{
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        weblog::logger.log(weblog::INFO, "Reactor is waiting for epoll events");
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

        if (nfds == -1)
        {
            weblog::logger.log(weblog::ERROR, "epoll_wait failed: " +
                                                  std::string(strerror(errno)));
            if (errno == EINTR)
                continue;
            throw std::runtime_error("epoll_wait failed");
        }

        weblog::logger.log(weblog::DEBUG, "Reactor received " +
                                              utils::to_string(nfds) +
                                              " events");
        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            if (_handlers.find(fd) == _handlers.end())
            {
                weblog::logger.log(weblog::ERROR, "No handler found for fd: " +
                                                      utils::to_string(fd));
                continue;
            }
            _handlers[fd]->handle_event(fd, events[i].events);
        }
    }
}

void Reactor::register_handler(int fd, IHandler* handler, uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    weblog::logger.log(weblog::DEBUG,
                       "Registering handler and set to non-blocking with fd: " +
                           utils::to_string(fd));
    utils::setup_nonblocking(fd);
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("epoll_ctl failed");
    _handlers[fd] = handler;
}

void Reactor::remove_handler(int fd)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("epoll_ctl failed");

    std::map<int, IHandler*>::iterator it = _handlers.find(fd);
    if (it != _handlers.end())
    {
        weblog::logger.log(weblog::DEBUG,
                           "Removed handler with fd: " + utils::to_string(fd));
        close(it->first);
        delete it->second;
        _handlers.erase(it);
    }
}

// EPOLL_CLOEXEC is a flag that makes sure that the file descriptor is closed
// when the process is replaced by another process
void Reactor::_init_epoll(void)
{
    if (_type == REACTOR)
        _epoll_fd = epoll_create1(0);
    else if (_type == WORKER)
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (_epoll_fd == -1)
        throw std::runtime_error("epoll_create1 failed");
    weblog::logger.log(weblog::DEBUG,
                       "Created epoll fd: " + utils::to_string(_epoll_fd));
}

} // namespace webkernel
