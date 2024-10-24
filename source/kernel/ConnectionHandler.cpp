#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <ctime>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{

ConnectionHandler::ConnectionHandler(Reactor* reactor)
    : _reactor(reactor), _processor(this)
{
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler& other)
    : _reactor(other._reactor), _processor(other._processor)
{
}

ConnectionHandler& ConnectionHandler::operator=(const ConnectionHandler& other)
{
    if (this != &other)
    {
        _reactor = other._reactor;
        _processor = other._processor;
    }
    return (*this);
}

ConnectionHandler::~ConnectionHandler()
{
    weblog::Logger::log(weblog::DEBUG, "ConnectionHandler destroyed");
    // TODO: Check the conn_fd is closed by the reactor
    // _handleClose(_conn_fd, weblog::INFO, "Connection closed by server");
}

void ConnectionHandler::handleEvent(int fd, uint32_t events)
{
    weblog::Logger::log(
        weblog::DEBUG,
        "ConnectionHandler::handleEvent() on fd: " + utils::toString(fd) +
            " with events: " + explainEpollEvent(events));
    if (events & EPOLLIN)
        _handleRead(fd);
    else if (events & EPOLLOUT)
        _handleWrite(fd);
    else if (events & EPOLLHUP)
        closeConnection(fd, weblog::INFO, "Connection closed by client");
    else if (events & EPOLLERR)
        closeConnection(fd, weblog::ERROR, "EPOLLERR received");
    else
        closeConnection(fd, weblog::ERROR,
                        "Handler got unknown event: " +
                            utils::toString(events));
}

void ConnectionHandler::closeConnection(int fd, weblog::LogLevel level,
                                        std::string message)
{
    weblog::Logger::log(level, message + " on conn_fd: " + utils::toString(fd));
    _reactor->removeHandler(fd);
}

void ConnectionHandler::prepareWrite(int fd, const std::string& buffer)
{
    struct epoll_event ev;

    weblog::Logger::log(weblog::DEBUG,
                        "Prepare to write " + utils::toString(buffer.size()) +
                            " bytes to fd: " + utils::toString(fd));
    // Modify the fd to be write events to notify the reactor to trigger the
    // response is ready to send
    ev.events = EPOLLOUT;
    // ev.events = EPOLLOUT | EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(_reactor->epollFd(), EPOLL_CTL_MOD, fd, &ev);
    _write_buffer[fd] = buffer;
}

/*
ConnectionHandler:
    1. the buffer is full, the request analyzer needs to comsume the buffer and
update the state
    2. EOF is reached, the current state of the request analyzer is the final
state
*/
void ConnectionHandler::_handleRead(int fd)
{
    char buffer[CHUNKED_SIZE];

    while (true)
    {
        ssize_t bytes_read = recv(fd, buffer, CHUNKED_SIZE, 0);

        _read_buffer[fd] += std::string(buffer, bytes_read);
        if (bytes_read > 0)
        {
            weblog::Logger::log(weblog::DEBUG,
                                "Read " + utils::toString(bytes_read) +
                                    " bytes from fd: " + utils::toString(fd));
            weblog::Logger::log(weblog::DEBUG,
                                "Buffer: \n" + std::string(buffer, bytes_read));
            _processor.analyze(_read_buffer[fd]);
            if (_processor.isRequestComplete())
            {
                weblog::Logger::log(weblog::INFO, "Request is complete");
                _processor.analyzeFinalize(fd);
                break;
            }
        }
        else if (bytes_read == 0)
        {
            _processor.analyzeFinalize(fd);
            closeConnection(fd, weblog::INFO,
                            "Read 0 bytes, client closing connection");
            break;
        }
        else
            closeConnection(fd, weblog::WARNING,
                            "Error to read, recv() failed: " +
                                std::string(strerror(errno)));
    }
}

void ConnectionHandler::_handleWrite(int fd)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Write event on fd: " + utils::toString(fd));
    std::map<int, std::string>::iterator it = _write_buffer.find(fd);

    if (it == _write_buffer.end())
    {
        weblog::Logger::log(weblog::ERROR, "No write buffer found for fd: " +
                                               utils::toString(fd));
        return;
    }
    else
    {
        weblog::Logger::log(weblog::DEBUG, "Write buffer found for fd: " +
                                               utils::toString(fd));
        int bytes_sent = send(fd, it->second.c_str(), it->second.size(), 0);
        if (bytes_sent < 0)
        {
            weblog::Logger::log(weblog::ERROR,
                                "Error to write, send() failed: " +
                                    std::string(strerror(errno)));
            closeConnection(fd, weblog::ERROR, "Error to write");
        }
        else if (bytes_sent == 0)
        {
            weblog::Logger::log(weblog::INFO,
                                "Write 0 bytes, client closing connection");
            // TODO: keep-alive?
            closeConnection(fd, weblog::INFO,
                            "Write 0 bytes, client closing connection");
        }
        else
        {
            weblog::Logger::log(weblog::DEBUG,
                                "Sent " + utils::toString(bytes_sent) +
                                    " bytes to fd: " + utils::toString(fd));
            _write_buffer.erase(it);

            // Modify the fd to be read events to notify the reactor to trigger
            // the next request
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = fd;
            epoll_ctl(_reactor->epollFd(), EPOLL_CTL_MOD, fd, &ev);
        }
    }
}

// TODO: Consider implementing the keep-alive timeout in the processor, because
// we need the request headers to determine the connection type
// bool ConnectionHandler::_keepAlive(void)
// {
//     time_t now = time(0);
//     double elapsed = difftime(now, _start_time);

//     weblog::Logger::log(weblog::DEBUG,
//                        "Elapsed time: " + utils::toString(elapsed));
//     // if (_request_parser.header_analyzer().connection_type() ==
//     // webshell::CLOSE)
//     //     return (false);
//     if (elapsed > _server_config.keepAliveTimeout())
//         return (false);

//     return (true);
// }

} // namespace webkernel
