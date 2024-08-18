#pragma once

#include "IHandler.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include <errno.h>
#include <iostream>
#include <string.h>

namespace webkernel
{

class ConnectionHandler : public IHandler
{
  public:
    ConnectionHandler();
    ConnectionHandler(int conn_fd, Reactor* reactor);
    ConnectionHandler(const ConnectionHandler& other);
    ConnectionHandler& operator=(const ConnectionHandler& other);
    ~ConnectionHandler();

    void handle_event(int fd, uint32_t events);

  private:
    int _conn_fd;
    Reactor* _reactor;
};

} // namespace webkernel
