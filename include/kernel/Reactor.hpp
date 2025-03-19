#pragma once
#include "IHandler.hpp"
#include "Singleton.hpp"
#include "defines.hpp"
#include <csignal>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

extern volatile sig_atomic_t stop_flag;

namespace webkernel
{

class Acceptor;
class ConnectionHandler;

class Reactor : public templates::Singleton<Reactor, ReactorType>
{
public:
    static Reactor* create_instance(); // Default instance creation
    static Reactor* create_instance(const ReactorType& type);

    void run(void);
    void register_handler(int fd, IHandler* handler, uint32_t events);
    void remove_handler(int fd);
    void
    modify_handler(int fd, uint32_t events_to_add, uint32_t events_to_remove);
    std::vector<int /* fd */> get_active_fds(void) const;
    void destroy_tree();
    bool handler_exists(int fd) const;

public:
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
    Reactor(const ReactorType& type);
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);
};

} // namespace webkernel
