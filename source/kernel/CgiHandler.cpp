#include "CgiHandler.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cstdlib>
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
    LOG(weblog::DEBUG,
        "CgiHandler::handle_event() on fd: " + utils::to_string(fd)
            + " with events: " + explain_epoll_event(events));

    if (events & EPOLLIN) {
        char buffer[CHUNKED_SIZE];
        std::memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(fd, buffer, CHUNKED_SIZE);

        if (bytes_read > 0) {
            _buffer.append(buffer, bytes_read);
        }
        else if (bytes_read == 0) {
            // Same as EPOLLHUP
            LOG(weblog::DEBUG, "EOF on fd: " + utils::to_string(fd));
            Reactor::instance()->remove_handler(fd);
            return;
        }
        else {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "read() failed.");
        }
        LOG(weblog::DEBUG,
            "Read " + utils::to_string(bytes_read)
                + " bytes from fd: " + utils::to_string(fd));
    }
    else if (events & EPOLLHUP) {
        LOG(weblog::DEBUG, "I only want to see this once!!!!!!!");
        LOG(weblog::DEBUG,
            "Waiting for child: " + utils::to_string(_pid)
                + " with events: " + explain_epoll_event(events));
        int status;
        int ret = waitpid(_pid, &status, WNOHANG);

        if (ret == 0) {
            LOG(weblog::DEBUG, "Child still running");
            return;
        }
        Reactor::instance()->remove_handler(fd);
        if (ret == -1) {
            webkernel::ConnectionHandler::instance()->prepare_error(
                _client_fd,
                utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                     "waitpid() failed."));
        }
        else {
            LOG(weblog::DEBUG,
                "Child exited with status: " + utils::to_string(status));
            // child has exited
            if (WIFEXITED(status)) {
                LOG(weblog::DEBUG, "Child exited normally");
                webkernel::ConnectionHandler::instance()->prepare_write(
                    _client_fd, _buffer);
            }
            else {
                LOG(weblog::DEBUG, "Child exited abnormally");
                webkernel::ConnectionHandler::instance()->prepare_error(
                    _client_fd,
                    utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                         "Child exited abnormally."));
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
