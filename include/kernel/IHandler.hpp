#pragma once

#include <sys/epoll.h>

namespace webkernel
{

class IHandler
{
public:
    virtual void handle_event(int fd, uint32_t events) = 0;

public:
    virtual ~IHandler() {}
};

} // namespace webkernel
