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
    Acceptor();
    Acceptor(Reactor* reactor, webconfig::Config* config);
    Acceptor(const Acceptor& other);
    Acceptor& operator=(const Acceptor& other);
    ~Acceptor();

    int create_connection(const char* ip, const char* port);
    void handleEvent(int fd, uint32_t events);
    void setup_server_id(int server_id);

  private:
    Reactor* _reactor;
    webconfig::Config* _config;
    int _server_id;
};

} // namespace webkernel
