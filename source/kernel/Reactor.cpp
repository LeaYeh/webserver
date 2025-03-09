#include "Reactor.hpp"
#include "CgiExecutor.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cerrno>
#include <cstdio>
#include <exception>
#include <unistd.h>
#include <vector>

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
Reactor::Reactor(const ReactorType& type) : _type(type)
{
    LOG(weblog::DEBUG, "Reactor::Reactor(" + utils::to_string(type) + ")");

    CgiExecutor::instantiate();
    if (_type == REACTOR) {
        _epoll_fd = epoll_create1(0);
        ConnectionHandler::instantiate();
    }
    else if (_type == WORKER) {
        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        ConnectionHandler::instantiate();
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

    LOG(weblog::DEBUG, "Created epoll fd: " + utils::to_string(_epoll_fd));
}

Reactor::~Reactor()
{
    LOG(weblog::DEBUG, "Reactor::~Reactor()");
    utils::safe_close(_epoll_fd);
    for (std::map<int, IHandler*>::iterator it = _handlers.begin();
         it != _handlers.end();
         it++) {
        close(it->first);
    }
    ConnectionHandler::destroy();
    CgiExecutor::destroy();
}

void Reactor::run(void)
{
    LOG(weblog::DEBUG, "Reactor::run()");
    struct epoll_event events[MAX_EVENTS];
    memset(events, 0, sizeof(events));

    while (true) {
        _check_interrupt();
        _wait_for_events(events);
    }
    LOG(weblog::DEBUG, "Reactor is shutting down");
}

// Add the handler to the map and register the fd with epoll
void Reactor::register_handler(int fd, IHandler* handler, uint32_t events)
{
    struct epoll_event ev = {.events = 0, .data = {0}};
    ev.events = events;
    ev.data.fd = fd;

    LOG(weblog::DEBUG,
        "Registering handler with fd: " + utils::to_string(fd)
            + ", events: " + explain_epoll_event(events));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed, "
                                 + std::string(strerror(errno)));
    }
    _handlers[fd] = handler;
}

void Reactor::modify_handler(int fd,
                             uint32_t events_to_add,
                             uint32_t events_to_remove)
{
    if (!handler_exists(fd))
    {
        LOG(weblog::CRITICAL, "Try to modify non-exist handler on fd: " + utils::to_string(fd));
        // close(fd);
        return;
    }
    struct epoll_event ev = {.events = 0, .data = {0}};

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        if (errno != ENOENT) {
            throw std::runtime_error("epoll_ctl failed to get current events, "
                                     + std::string(strerror(errno))
                                     + " for fd: " + utils::to_string(fd));
        }
        ev.events = 0;
    }

    ev.events |= events_to_add;
    ev.events &= ~events_to_remove;
    ev.data.fd = fd;
    LOG(weblog::DEBUG,
        "Modifying handler with fd: " + utils::to_string(fd)
            + ", new events: " + explain_epoll_event(ev.events));

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        throw std::runtime_error("epoll_ctl failed to modify events, "
                                 + std::string(strerror(errno)));
    }
}

void Reactor::destroy_tree()
{
    close(_epoll_fd);
}

bool Reactor::handler_exists(int fd) const {
    std::map<int, IHandler*>::const_iterator it = _handlers.find(fd);
    return (it != _handlers.end());
}

void Reactor::remove_handler(int fd)
{
    if (!handler_exists(fd))
    {
        LOG(weblog::CRITICAL, "Try to remove non-exist handle on fd: " + utils::to_string(fd));
        return;
    }
    LOG(weblog::DEBUG, "Removed handler with fd: " + utils::to_string(fd));
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
        throw std::runtime_error("epoll_ctl(EPOLL_CTL_DEL) failed on "
                                    + utils::to_string(fd) + ": "
                                    + std::string(strerror(errno)));
    }
    close(fd);
    // delete it->second;
    _handlers.erase(fd);
}

std::vector<int> Reactor::get_active_fds(void) const
{
    std::vector<int> fds;
    for (std::map<int, IHandler*>::const_iterator it = _handlers.begin();
         it != _handlers.end();
         it++) {
        fds.push_back(it->first);
    }
    return fds;
}

void Reactor::_check_interrupt(void) const
{
    if (stop_flag) {
        LOG(weblog::INFO, "Reactor received interrupt signal");
        throw InterruptException();
    }
}

void Reactor::_wait_for_events(struct epoll_event* events)
{
    LOG(weblog::DEBUG,
        "Reactor is waiting for events: "
            + explain_epoll_event(events->events));
    int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);

    if (nfds == -1 && !stop_flag) {
        LOG(weblog::ERROR,
            "epoll_wait failed: " + std::string(strerror(errno)));
        throw std::runtime_error("epoll_wait failed");
    }
    LOG(weblog::DEBUG,
        "Reactor received " + utils::to_string(nfds) + " events");
    _handle_events(events, nfds);
}

void Reactor::_handle_events(struct epoll_event* events, int nfds)
{
    for (int i = 0; i < nfds; i++) {
        int fd = events[i].data.fd;

        if (!handler_exists(fd))
        {
            LOG(weblog::CRITICAL, "Try to process non-exist handler on fd: " + utils::to_string(fd));
            continue;
        }
        try {
            LOG(weblog::DEBUG,
                "Handling event on fd: " + utils::to_string(fd)
                    + " with events: " + explain_epoll_event(events[i].events));
            for (std::map<int, IHandler*>::iterator it = _handlers.begin();
                 it != _handlers.end();
                 it++) {
                LOG(weblog::WARNING,
                    "activated fd: " + utils::to_string(it->first));
            }
            LOG(weblog::WARNING,
                "using fd: " + utils::to_string(fd)
                    + " on handler: " + utils::to_string(_handlers[fd]));
            _handlers[fd]->handle_event(fd, events[i].events);
        }
        catch (utils::HttpException& e) {
            LOG(weblog::ERROR, e.what());
            ConnectionHandler::instance()->prepare_error(fd, e);
        }
        catch (std::exception& e) {
            LOG(weblog::ERROR, e.what());
            throw;
        }
    }
}

} // namespace webkernel
