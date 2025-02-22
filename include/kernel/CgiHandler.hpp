#include "IHandler.hpp"
#include "Request.hpp"
#include <cctype>
#include <sys/epoll.h>

namespace webkernel
{
class CgiHandler : public IHandler
{
public:
    CgiHandler(int client_fd, pid_t pid);
    ~CgiHandler();
    void handle_event(int fd, uint32_t events);

private:
    // webshell::Request& _request;
    std::string _buffer;
    int _client_fd;
    pid_t _pid;
};
} // namespace webkernel
