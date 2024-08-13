#pragma once

#include "Handler.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

namespace webkernel
{

class Reactor
{
  public:
    Reactor();
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);
    ~Reactor();
};

} // namespace webkernel
