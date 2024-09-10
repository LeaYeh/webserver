#pragma once

#include "IHandler.hpp"
#include "defines.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <sys/epoll.h>
#include <csignal>
#include <exception>
#include <unistd.h>

extern volatile sig_atomic_t stop_flag;

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

    class InterruptException : public std::exception
    {
      public:
        const char* what() const throw()
        {
            return STY_GRE "Shutdown webserver" STY_RES;
        }
    };

  private:
    ReactorType _type;
    int _epoll_fd;
    std::map<int, IHandler*> _handlers;

    void _init_epoll(void);
};

} // namespace webkernel
