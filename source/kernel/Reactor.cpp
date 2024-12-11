#include "Reactor.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <cerrno>
#include <cstdio>
#include <unistd.h>

namespace webkernel
{

// EPOLL_CLOEXEC is a flag that makes sure that the file descriptor is closed
// when the process is replaced by another process
Reactor::Reactor(ReactorType type) : conn_handler(NULL), _type(type)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor::Reactor(" + utils::toString(type) + ")");
    if (_type == REACTOR)
    {
        _epoll_fd = epoll_create1(0);
        conn_handler = new ConnectionHandler(this);
    }
    else if (_type == WORKER)
    {
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        conn_handler = new ConnectionHandler(this);
    }
    else if (_type == DISPATCHER)
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    else
        throw std::runtime_error("Invalid reactor type");
    if (_epoll_fd == -1)
        throw std::runtime_error("epoll_create1 failed");
    weblog::Logger::log(weblog::DEBUG,
                        "Created epoll fd: " + utils::toString(_epoll_fd));
}

Reactor::Reactor(const Reactor& other)
    : _type(other._type), _epoll_fd(other._epoll_fd), _handlers(other._handlers)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor::Reactor(const Reactor& other)");
}

Reactor& Reactor::operator=(const Reactor& other)
{
    weblog::Logger::log(weblog::DEBUG,
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
    weblog::Logger::log(weblog::DEBUG, "Reactor::~Reactor()");
    utils::safeClose(_epoll_fd);
    for (std::map<int, IHandler*>::iterator it = _handlers.begin();
         it != _handlers.end(); it++)
        close(it->first);
    if (conn_handler)
        delete conn_handler;
}

void Reactor::run(void)
{
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        if (stop_flag)
        {
            weblog::Logger::log(weblog::INFO,
                                "Reactor received interrupt signal");
            throw InterruptException();
        }
        weblog::Logger::log(weblog::INFO,
                            "Reactor is waiting for epoll events");
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

        if (nfds == -1 && !stop_flag)
        {
            weblog::Logger::log(weblog::ERROR,
                                "epoll_wait failed: " +
                                    std::string(strerror(errno)));
            // TODO: the errno is forbidden by the subject, maybe we should just
            // continue and assume the error case never happens
            if (errno == EINTR)
                continue;
            throw std::runtime_error("epoll_wait failed");
        }

        weblog::Logger::log(weblog::DEBUG, "Reactor received " +
                                               utils::toString(nfds) +
                                               " events");
        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            if (_handlers.find(fd) == _handlers.end())
                throw std::runtime_error("Handler not found for fd: " +
                                         utils::toString(fd));
            try
            {
                _handlers[fd]->handleEvent(fd, events[i].events);
            }
            catch (utils::HttpException& e)
            {
                weblog::Logger::log(weblog::WARNING, e.what());
                conn_handler->prepareError(fd, e);
            }
        }
    }
    weblog::Logger::log(weblog::DEBUG, "Reactor is shutting down");
}

// Add the handler to the map and register the fd with epoll
void Reactor::registerHandler(int fd, int server_id, IHandler* handler,
                              uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    weblog::Logger::log(weblog::DEBUG,
                        "Registering handler with fd: " + utils::toString(fd));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
        throw std::runtime_error("epoll_ctl failed");
    _handlers[fd] = handler;
    _server_map[fd] = server_id;
}

void Reactor::modifyHandler(int fd, uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    weblog::Logger::log(weblog::DEBUG,
                        "Modifying handler with fd: " + utils::toString(fd));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
        throw std::runtime_error("epoll_ctl failed");
}

void Reactor::removeHandler(int fd)
{
    std::map<int, IHandler*>::iterator it = _handlers.find(fd);
    if (it != _handlers.end())
    {
        weblog::Logger::log(weblog::DEBUG,
                            "Removed handler with fd: " + utils::toString(fd));
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
            throw std::runtime_error("epoll_ctl(EPOLL_CTL_DEL) failed on " +
                                     utils::toString(fd) + ": " +
                                     std::string(strerror(errno)));
        close(it->first);
        _handlers.erase(it);
    }
}

int Reactor::lookupServerId(int fd) const
{
    std::map<int, int>::const_iterator it = _server_map.find(fd);
    if (it != _server_map.end())
        return (it->second);
    return (-1);
}

int Reactor::epollFd(void) const
{
    return (_epoll_fd);
}

} // namespace webkernel
