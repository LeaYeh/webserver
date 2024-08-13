#pragma once

#include "MainReactor.hpp"
#include "Reactor.hpp"
#include "SubReactor.hpp"
#include "ServerConfig.hpp"

namespace webkernel
{

class Kernel
{
  public:
    Kernel();
    Kernel(const Kernel&);
    Kernel& operator=(const Kernel&);
    ~Kernel();

    void run();

  private:
    webconfig::ServerConfig _config;
    void _create_reactors();
};

} // namespace webkernel
