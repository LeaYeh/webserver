#include "defines.hpp"
#include <iostream>
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{
std::string explainEpollEvent(uint32_t events);
} // namespace webkernel
