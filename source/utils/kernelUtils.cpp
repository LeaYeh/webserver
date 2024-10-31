#include "kernelUtils.hpp"
#include "Config.hpp"
#include "ConfigServerBlock.hpp"

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

bool checkMethodLimit(webshell::Request request,
                      const std::vector<webshell::RequestMethod>& limit_except)
{
    for (std::size_t i = 0; i < limit_except.size(); ++i)
    {
        if (request.method() == limit_except[i])
            return (true);
    }
    return (false);
}

bool checkPathFormat(const std::string& path)
{
    if (path[0] != '/')
        return (false);
    if (path.size() > MAX_PATH_LENGTH)
        return (false);
    if (path.find("..") != std::string::npos)
        return (false);
    if (path.find("//") != std::string::npos)
        return (false);
    if (path.find("~") != std::string::npos)
        return (false);
    return (true);
}

} // namespace webkernel
