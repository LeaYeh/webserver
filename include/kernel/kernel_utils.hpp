#include <sys/socket.h>
#include <iostream>
#include <stdint.h>
#include <sys/epoll.h>
#include "defines.hpp"

namespace webkernel
{
    std::string explain_epoll_event(uint32_t events);
} // namespace webkernel
