#pragma once

#include "IHandler.hpp"
#include "defines.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

namespace webkernel
{

class Acceptor;

class Reactor
{
  public:
    Reactor();
    Reactor(ReactorType type);
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);
    ~Reactor();

    void run(void);
    void register_handler(int fd, IHandler* handler, uint32_t events);
    void remove_handler(int fd);

  private:
    ReactorType _type;
    int _epoll_fd;
    std::map<int, IHandler*> _handlers;

    void _init_epoll(void);
};

} // namespace webkernel
