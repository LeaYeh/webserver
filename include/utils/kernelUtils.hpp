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
std::pair<webshell::StatusCode, std::string /* reasonDetail */>
checkRequestPath(webshell::Request& request, int server_id);
bool checkMethodLimit(webshell::Request request,
                      const std::vector<webshell::RequestMethod>& limit_except);
bool checkPathFormat(const std::string& path);

} // namespace webkernel
