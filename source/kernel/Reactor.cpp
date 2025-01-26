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

Reactor* Reactor::create_instance()
{
    return new Reactor(REACTOR); // Default instance creation is REACTOR
}

Reactor* Reactor::create_instance(const ReactorType& type)
{
    return new Reactor(type);
}

// EPOLL_CLOEXEC is a flag that makes sure that the file descriptor is closed
// when the process is replaced by another process
Reactor::Reactor(const ReactorType& type) : conn_handler(NULL), _type(type)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor::Reactor(" + utils::to_string(type) + ")");
    if (_type == REACTOR) {
        _epoll_fd = epoll_create1(0);
        conn_handler = new ConnectionHandler();
    }
    else if (_type == WORKER) {
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        conn_handler = new ConnectionHandler();
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
                        "Created epoll fd: " + utils::to_string(_epoll_fd));
}

Reactor::~Reactor()
{
    weblog::Logger::log(weblog::DEBUG, "Reactor::~Reactor()");
    utils::safe_close(_epoll_fd);
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
void Reactor::register_handler(int fd, IHandler* handler, uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    weblog::Logger::log(weblog::DEBUG,
                        "Registering handler with fd: " + utils::to_string(fd));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed");
    }
    _handlers[fd] = handler;
}

void Reactor::modify_handler(int fd,
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
                        "Modifying handler with fd: " + utils::to_string(fd)
                            + ", new events: "
                            + explain_epoll_event(ev.events));

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed to modify events");
    }
}

void Reactor::remove_handler(int fd)
{
    std::map<int, IHandler*>::iterator it = _handlers.find(fd);
    if (it != _handlers.end()) {
        weblog::Logger::log(weblog::DEBUG,
                            "Removed handler with fd: " + utils::to_string(fd));
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            throw std::runtime_error("epoll_ctl(EPOLL_CTL_DEL) failed on "
                                     + utils::to_string(fd) + ": "
                                     + std::string(strerror(errno)));
        }
        close(it->first);
        _handlers.erase(it);
    }
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
                            + explain_epoll_event(events->events));
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

    if (nfds == -1 && !stop_flag) {
        weblog::Logger::log(weblog::ERROR,
                            "epoll_wait failed: "
                                + std::string(strerror(errno)));
        throw std::runtime_error("epoll_wait failed");
    }
    weblog::Logger::log(weblog::DEBUG,
                        "Reactor received " + utils::to_string(nfds)
                            + " events");
    _handle_events(events, nfds);
}

void Reactor::_handle_events(struct epoll_event* events, int nfds)
{
    for (int i = 0; i < nfds; i++) {
        int fd = events[i].data.fd;

        if (_handlers.find(fd) == _handlers.end()) {
            throw std::runtime_error("Handler not found for fd: "
                                     + utils::to_string(fd));
        }
        try {
            _handlers[fd]->handle_event(fd, events[i].events);
        }
        catch (utils::HttpException& e) {
            weblog::Logger::log(weblog::ERROR, e.what());
            conn_handler->prepare_error(fd, e);
        }
    }
}

} // namespace webkernel
