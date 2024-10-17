#pragma once

#include <sys/epoll.h>

namespace webkernel
{

class IHandler
{
  public:
    virtual ~IHandler()
    {
    }

    virtual void handleEvent(int fd, uint32_t events) = 0;
};

} // namespace webkernel
