#pragma once

#include "IHandler.hpp"
#include "Reactor.hpp"
#include "ConnectionHandler.hpp"
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

namespace webkernel
{

class Acceptor : public IHandler
{
  public:
    Acceptor();
    Acceptor(Reactor* reactor);
    Acceptor(const Acceptor& other);
    Acceptor& operator=(const Acceptor& other);
    ~Acceptor();

    int create_connection(const char* ip, const char* port);
    void handle_event(int fd, uint32_t events);

  private:
    Reactor* _reactor;
};

} // namespace webkernel
