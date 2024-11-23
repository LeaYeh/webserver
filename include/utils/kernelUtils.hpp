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

} // namespace webkernel
