#pragma once

#include "IHandler.hpp"
#include "defines.hpp"
#include <csignal>
#include <exception>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>

extern volatile sig_atomic_t stop_flag;

namespace webkernel
{

class Acceptor;
class ConnectionHandler;

class Reactor
{
public:
    ConnectionHandler* conn_handler;

public:
    void run(void);
    void registerHandler(int fd, IHandler* handler, uint32_t events);
    void removeHandler(int fd);
    void
    modifyHandler(int fd, uint32_t events_to_add, uint32_t events_to_remove);
    int epollFd(void) const;

    class InterruptException : public std::exception
    {
    public:
        const char* what() const throw()
        {
            return STY_GRE "Shutdown webserver" STY_RES;
        }
    };

public:
    Reactor(ReactorType type);
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);
    ~Reactor();

private:
    ReactorType _type;
    int _epoll_fd;
    std::map<int /* fd */, IHandler*> _handlers;

private:
    void _check_interrupt(void) const;
    void _wait_for_events(struct epoll_event* events);
    void _handle_events(struct epoll_event* events, int nfds);

private:
    Reactor();
};

} // namespace webkernel
