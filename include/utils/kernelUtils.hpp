#include <stdint.h>
#include <string>
#include <sys/epoll.h>
#include "defines.hpp"

namespace webkernel
{

std::string explainEpollEvent(uint32_t events);
std::string explainEventProcessingState(EventProcessingState state);

} // namespace webkernel
