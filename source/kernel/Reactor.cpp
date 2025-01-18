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
    if (_type == REACTOR) {
        _epoll_fd = epoll_create1(0);
        conn_handler = new ConnectionHandler(this);
    }
    else if (_type == WORKER) {
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        conn_handler = new ConnectionHandler(this);
    }
    else if (_type == DISPATCHER) {
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    }
    else {
        throw std::runtime_error("Invalid reactor type");
    }
    if (_epoll_fd == -1) {
        throw std::runtime_error("epoll_create1 failed");
    }
    weblog::Logger::log(weblog::DEBUG,
                        "Created epoll fd: " + utils::toString(_epoll_fd));
}

Reactor::Reactor(const Reactor& other) :
    conn_handler(other.conn_handler),
    _type(other._type),
    _epoll_fd(other._epoll_fd),
    _handlers(other._handlers)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor::Reactor(const Reactor& other)");
}

Reactor& Reactor::operator=(const Reactor& other)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor::operator=(const Reactor& other)");
    if (this != &other) {
        conn_handler = other.conn_handler;
        _type = other._type;
        _epoll_fd = other._epoll_fd;
        _handlers = other._handlers;
    }
    return (*this);
}

Reactor::~Reactor()
{
    weblog::Logger::log(weblog::DEBUG, "Reactor::~Reactor()");
    utils::safeClose(_epoll_fd);
    for (std::map<int, IHandler*>::iterator it = _handlers.begin();
         it != _handlers.end();
         it++) {
        close(it->first);
    }
    if (conn_handler) {
        delete conn_handler;
    }
}

void Reactor::run(void)
{
    weblog::Logger::log(weblog::DEBUG, "Reactor::run()");
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        _check_interrupt();
        _wait_for_events(events);
    }
    weblog::Logger::log(weblog::DEBUG, "Reactor is shutting down");
}

// Add the handler to the map and register the fd with epoll
void Reactor::registerHandler(int fd, IHandler* handler, uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    weblog::Logger::log(weblog::DEBUG,
                        "Registering handler with fd: " + utils::toString(fd));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed");
    }
    _handlers[fd] = handler;
}

void Reactor::modifyHandler(int fd,
                            uint32_t events_to_add,
                            uint32_t events_to_remove)
{
    struct epoll_event ev = {.events = 0, .data = {0}};

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        if (errno != ENOENT) {
            throw std::runtime_error("epoll_ctl failed to get current events");
        }
        ev.events = 0;
    }

    ev.events |= events_to_add;
    ev.events &= ~events_to_remove;
    ev.data.fd = fd;
    weblog::Logger::log(weblog::DEBUG,
                        "Modifying handler with fd: " + utils::toString(fd)
                            + ", new events: " + explainEpollEvent(ev.events));

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed to modify events");
    }
}

void Reactor::removeHandler(int fd)
{
    std::map<int, IHandler*>::iterator it = _handlers.find(fd);
    if (it != _handlers.end()) {
        weblog::Logger::log(weblog::DEBUG,
                            "Removed handler with fd: " + utils::toString(fd));
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            throw std::runtime_error("epoll_ctl(EPOLL_CTL_DEL) failed on "
                                     + utils::toString(fd) + ": "
                                     + std::string(strerror(errno)));
        }
        close(it->first);
        _handlers.erase(it);
    }
}

int Reactor::epollFd(void) const
{
    return (_epoll_fd);
}

void Reactor::_check_interrupt(void) const
{
    if (stop_flag) {
        weblog::Logger::log(weblog::INFO, "Reactor received interrupt signal");
        throw InterruptException();
    }
}

void Reactor::_wait_for_events(struct epoll_event* events)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor is waiting for events: "
                            + explainEpollEvent(events->events));
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

    if (nfds == -1 && !stop_flag) {
        weblog::Logger::log(weblog::ERROR,
                            "epoll_wait failed: "
                                + std::string(strerror(errno)));
        throw std::runtime_error("epoll_wait failed");
    }
    weblog::Logger::log(
        weblog::DEBUG, "Reactor received " + utils::toString(nfds) + " events");
    _handle_events(events, nfds);
}

void Reactor::_handle_events(struct epoll_event* events, int nfds)
{
    for (int i = 0; i < nfds; i++) {
        int fd = events[i].data.fd;

        if (_handlers.find(fd) == _handlers.end()) {
            throw std::runtime_error("Handler not found for fd: "
                                     + utils::toString(fd));
        }
        try {
            _handlers[fd]->handleEvent(fd, events[i].events);
        }
        catch (utils::HttpException& e) {
            weblog::Logger::log(weblog::ERROR, e.what());
            conn_handler->prepareError(fd, e);
        }
    }
}

} // namespace webkernel
