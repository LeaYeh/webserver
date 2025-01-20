#include "defines.hpp"
#include <stdint.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{

std::string explainEpollEvent(uint32_t events);
std::string get_object_size(const std::string& path);
std::string get_object_mtime(const std::string& path);
std::string format_size(uint64_t size);
std::string format_time(time_t time);
std::string get_object_type(const std::string& path);
std::string explainEventProcessingState(EventProcessingState state);
std::string uuid();
std::string get_socket_address(int fd);
std::string get_socket_address(const struct sockaddr_storage& addr);
std::string format_address(const struct sockaddr_storage& addr);

} // namespace webkernel
