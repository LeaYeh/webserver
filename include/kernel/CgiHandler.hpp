#pragma once
#include "IHandler.hpp"
#include <string>
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

private:
    void _read_pipe(int fd);
    CgiHandler(const CgiHandler& other);
    CgiHandler& operator=(const CgiHandler& other);

};
} // namespace webkernel
