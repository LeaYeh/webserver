#pragma once

#include "IHandler.hpp"
#include "defines.hpp"
#include "utils/utils.hpp"
#include "Config.hpp"
#include <csignal>
#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
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
    Reactor(ReactorType type);
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);
    ~Reactor();

    void run(void);
    void registerHandler(int fd, int server_id, IHandler* handler, uint32_t events);
    void removeHandler(int fd);
    void modifyHandler(int fd, uint32_t events_to_add, uint32_t events_to_remove);
    int lookupServerId(int fd) const;
    int epollFd(void) const;

    ConnectionHandler* conn_handler;

    class InterruptException : public std::exception
    {
      public:
        const char* what() const throw()
        {
            return STY_GRE "Shutdown webserver" STY_RES;
        }
    };

  private:
    Reactor();
    ReactorType _type;
    int _epoll_fd;
    std::map<int /* fd */, IHandler*> _handlers;
    std::map<int /* fd */, int /* server_id */> _server_map;
};

} // namespace webkernel
