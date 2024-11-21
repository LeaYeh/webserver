#include "kernelUtils.hpp"

namespace webkernel
{
std::string explainEpollEvent(uint32_t events)
{
    std::string explanation;

    if (events & EPOLLIN)
        explanation += "EPOLLIN ";
    if (events & EPOLLOUT)
        explanation += "EPOLLOUT ";
    if (events & EPOLLRDHUP)
        explanation += "EPOLLRDHUP ";
    if (events & EPOLLERR)
        explanation += "EPOLLERR ";
    if (events & EPOLLHUP)
        explanation += "EPOLLHUP ";
    if (events & EPOLLET)
        explanation += "EPOLLET ";
    if (events & EPOLLONESHOT)
        explanation += "EPOLLONESHOT ";

    return (explanation);
}

std::string explainEventProcessingState(EventProcessingState state)
{
    std::string explanation;

    if (state & INITIAL)
        explanation += "INITIAL ";
    if (state & PROCESSING)
        explanation += "PROCESSING ";
    if (state & COMPELETED)
        explanation += "COMPELETED ";
    if (state & ERROR)
        explanation += "ERROR ";
    if (state & WRITE_CHUNKED)
        explanation += "WRITE_CHUNKED ";
    if (state & WRITE_OTHERS_CHUNKED)
        explanation += "WRITE_OTHERS_CHUNKED ";

    return (explanation);
}

} // namespace webkernel
