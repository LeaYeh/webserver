#pragma once

#include <sys/epoll.h>
#include <unistd.h>

namespace webkernel
{

class Handler
{
  public:
    Handler();
    Handler(const Handler&);
    Handler& operator=(const Handler&);
    ~Handler();

    void handle(int fd, uint32_t events);
};

} // namespace webkernel
