#pragma once

#include "Config.hpp"
#include "Reactor.hpp"
#include "Acceptor.hpp"
#include "defines.hpp"
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>

namespace webkernel
{

class Kernel
{
  public:
    Kernel(webconfig::Config& config);
    Kernel(const Kernel& other);
    Kernel& operator=(const Kernel& other);
    ~Kernel();

    void run(void);

  private:
    Kernel();
    webconfig::Config _config;
    Reactor *_reactor;
    Acceptor *_acceptor;

    void _init(void);
    void _init_listener(void);
    int _create_listen_socket(const char* ip, const char* port);
    void _create_single_reactor_single_worker(void);
    void _create_multi_reactor_multi_worker(void);
};

} // namespace webkernel
