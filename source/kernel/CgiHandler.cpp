#include "CgiHandler.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace webkernel
{

CgiHandler::CgiHandler(int client_fd, pid_t pid) :
    _buffer(""), _client_fd(client_fd), _pid(pid)
{
}

CgiHandler::~CgiHandler() {}

void CgiHandler::handle_event(int fd /*read_end*/, uint32_t events)
{
    LOG(weblog::CRITICAL,
        "CgiHandler::handle_event() on fd: " + utils::to_string(fd)
            + " with events: " + explain_epoll_event(events));

    if (events & EPOLLIN) {
        char buffer[CHUNKED_SIZE];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(fd, buffer, CHUNKED_SIZE);

        std::cout << "bytes_read: " << bytes_read << std::endl;
        if (bytes_read > 0) {
            _buffer.append(buffer, bytes_read);
        }
        else {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "read() failed: "
                                           + std::string(strerror(errno)));
        }
    }
    else if (events & EPOLLOUT) {
        LOG(weblog::CRITICAL, "Waiting for child: " + utils::to_string(_pid));
        int status;
        int ret = waitpid(_pid, &status, WNOHANG);

        if (ret == -1) {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "waitpid() failed: "
                                           + std::string(strerror(errno)));
        }
        else if (ret == 0) {
            LOG(weblog::CRITICAL, "Child still running");
            // child is still running
            return;
        }
        else {
            LOG(weblog::CRITICAL, "Child exited with status: "
                                       + utils::to_string(status));
            // child has exited
            if (WIFEXITED(status)) {
                LOG(weblog::CRITICAL, "Child exited normally");
                webkernel::ConnectionHandler::instance()->prepare_write(
                    _client_fd, _buffer);
                webkernel::Reactor::instance()->remove_handler(fd);
            }
            else {
                LOG(weblog::CRITICAL, "Child exited abnormally");
                throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                           "Child exited abnormally");
            }
        }
    }
    else {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: "
                                       + utils::to_string(fd));
    }
}

} // namespace webkernel
