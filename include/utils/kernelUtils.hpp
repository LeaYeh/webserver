#include "defines.hpp"
#include <cstddef>
#include <stdint.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{

std::string explain_epoll_event(uint32_t events);
std::string get_object_size(const std::string& path);
size_t get_file_size(const std::string& path);
std::string get_object_mtime(const std::string& path);
std::string format_size(uint64_t size);
std::string format_time(time_t time);
std::string get_object_type(const std::string& path);
std::string explain_event_processing_state(EventProcessingState state);
std::string uuid();
std::string get_socket_address(int fd);
std::string get_socket_address(const struct sockaddr_storage& addr);
std::string get_client_address(int fd);
std::string get_client_address(const struct sockaddr_storage& addr);
std::string format_address(const struct sockaddr_storage& addr);

} // namespace webkernel
