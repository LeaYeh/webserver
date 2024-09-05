#include "kernel_utils.hpp"

namespace webkernel
{
    std::string explain_epoll_event(uint32_t events)
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
} // namespace webkernel
