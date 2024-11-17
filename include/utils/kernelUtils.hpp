#include "Request.hpp"
#include "defines.hpp"
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <utility>
#include <vector>

namespace webkernel
{

std::string explainEpollEvent(uint32_t events);

} // namespace webkernel
