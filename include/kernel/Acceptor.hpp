#pragma once

#include "Config.hpp"
#include "ConnectionHandler.hpp"
#include "IHandler.hpp"
#include "Reactor.hpp"
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
    Acceptor(Reactor* reactor);
    Acceptor(const Acceptor& other);
    Acceptor& operator=(const Acceptor& other);
    ~Acceptor();

    void handleEvent(int fd, uint32_t events);

  private:
    Acceptor();
    Reactor* _reactor;
};

} // namespace webkernel
