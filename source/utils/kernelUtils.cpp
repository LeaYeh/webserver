#include "kernelUtils.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <errno.h>
#include <string>
#include <sys/socket.h>

namespace webkernel
{
std::string explainEpollEvent(uint32_t events)
{
    std::string explanation;

    if (events & EPOLLIN) {
        explanation += "EPOLLIN ";
    }
    if (events & EPOLLOUT) {
        explanation += "EPOLLOUT ";
    }
    if (events & EPOLLRDHUP) {
        explanation += "EPOLLRDHUP ";
    }
    if (events & EPOLLERR) {
        explanation += "EPOLLERR ";
    }
    if (events & EPOLLHUP) {
        explanation += "EPOLLHUP ";
    }
    if (events & EPOLLET) {
        explanation += "EPOLLET ";
    }
    if (events & EPOLLONESHOT) {
        explanation += "EPOLLONESHOT ";
    }

    return (explanation);
}

std::string get_object_size(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1) {
        return ("0");
    }
    return (format_size(file_stat.st_size));
}

std::string get_object_mtime(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1) {
        return ("0");
    }
    return (format_time(file_stat.st_mtime));
}

std::string get_object_type(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1) {
        return ("0");
    }
    if (S_ISDIR(file_stat.st_mode)) {
        return ("directory");
    }
    if (S_ISREG(file_stat.st_mode)) {
        return ("file");
    }
    return ("unknown");
}

std::string format_size(uint64_t size)
{
    std::string formatted_size;
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int UNITS_COUNT = sizeof(units) / sizeof(units[0]);

    for (int i = 0; i < UNITS_COUNT; i++) {
        if (size < 1024) {
            formatted_size = utils::toString(size) + " " + units[i];
            break;
        }
        size /= 1024;
    }
    return (formatted_size);
}

std::string format_time(time_t time)
{
    char buffer[80];

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&time));
    return (std::string(buffer));
}

std::string explainEventProcessingState(EventProcessingState state)
{
    std::string explanation;

    if (state & INITIAL) {
        explanation += "INITIAL ";
    }
    if (state & PROCESSING) {
        explanation += "PROCESSING ";
    }
    if (state & COMPELETED) {
        explanation += "COMPELETED ";
    }
    if (state & ERROR) {
        explanation += "ERROR ";
    }
    if (state & HANDLE_CHUNKED) {
        explanation += "HANDLE_CHUNKED ";
    }
    if (state & HANDLE_OTHERS_CHUNKED) {
        explanation += "HANDLE_OTHERS_CHUNKED ";
    }

    return (explanation);
}

std::string uuid()
{
    std::time_t now = std::time(0);
    std::srand(static_cast<unsigned int>(now));
    int rand_num = std::rand();

    return (utils::toString(now) + "_" + utils::toString(rand_num));
}

std::string get_socket_address(int fd)
{
    struct sockaddr_storage addr;
    socklen_t addr_size = sizeof(addr);

    if (getsockname(fd, (struct sockaddr*)&addr, &addr_size) < 0) {
        throw std::runtime_error("getsockname() failed: "
                                 + std::string(strerror(errno)));
    }
    return (format_address(addr));
}

std::string get_socket_address(const struct sockaddr_storage& addr)
{
    return (format_address(addr));
}

std::string format_address(const struct sockaddr_storage& addr)
{
    std::ostringstream oss;

    // IPv4 address
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* in4 = (struct sockaddr_in*)&addr;
        char buf[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET, &in4->sin_addr, buf, sizeof(buf)) == NULL) {
            throw std::runtime_error("inet_ntop() failed: "
                                     + std::string(strerror(errno)));
        }
        oss << buf << ":" << ntohs(in4->sin_port);
    }
    // IPv6 address
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* in6 = (struct sockaddr_in6*)&addr;
        char buf[INET6_ADDRSTRLEN];

        if (inet_ntop(AF_INET6, &in6->sin6_addr, buf, sizeof(buf)) == NULL) {
            throw std::runtime_error("inet_ntop() failed: "
                                     + std::string(strerror(errno)));
        }
        oss << buf << ntohs(in6->sin6_port);
    }
    else {
        throw std::runtime_error("Unknown address family");
    }
    return oss.str();
}

} // namespace webkernel
