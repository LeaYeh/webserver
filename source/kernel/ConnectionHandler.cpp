#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <exception>
#include <sys/epoll.h>

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

const Reactor* ConnectionHandler::reactor(void) const
{
    return (_reactor);
}

void ConnectionHandler::handleEvent(int fd, uint32_t events)
{
    weblog::Logger::log(
        weblog::DEBUG,
        "ConnectionHandler::handleEvent() on fd: " + utils::toString(fd) +
            " with events: " + explainEpollEvent(events));
    weblog::Logger::log(weblog::DEBUG,
                        "Received events: " + explainEpollEvent(events) +
                            " on fd: " + utils::toString(fd));
    if (events & EPOLLIN)
        _handleRead(fd);
    else if (events & EPOLLHUP)
        closeConnection(fd, weblog::INFO, "Connection closed by client");
    else if (events & EPOLLERR)
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "EPOLLERR event on fd: " +
                                       utils::toString(fd));
    else if (events & EPOLLOUT)
        _handleWrite(fd);
    else
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: " +
                                       utils::toString(fd));
}

void ConnectionHandler::closeConnection(int fd, weblog::LogLevel level,
                                        std::string message)
{
    weblog::Logger::log(level, message + " on conn_fd: " + utils::toString(fd));
    _reactor->removeHandler(fd);
    _processor.removeAnalyzer(fd);
    _read_buffer.erase(fd);
    _write_buffer.erase(fd);
}

void ConnectionHandler::prepareWrite(int fd, const std::string& buffer)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Prepare to write " + utils::toString(buffer.size()) +
                            " bytes to fd: " + utils::toString(fd));
    _write_buffer[fd] = buffer;
}

void ConnectionHandler::prepareError(int fd, webshell::StatusCode status_code,
                                     const std::string& description)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Prepare to write error response to fd: " +
                            utils::toString(fd));
    // TODO: Here might occour OOM, need to consider closing the connection
    // directly. ref: PR#65
    webshell::Response err_response =
        webshell::ResponseBuilder::buildErrorResponse(status_code, description);

    weblog::Logger::log(weblog::CRITICAL,
                        "Error response: \n" + err_response.serialize());
    _error_buffer[fd] = err_response.serialize();
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
            if (_processor.analyze(fd, _read_buffer[fd]))
                break;
        }
        else if (bytes_read == 0)
        {
            _processor.analyze(fd, _read_buffer[fd]);
            closeConnection(fd, weblog::INFO,
                            "Read 0 bytes, client closing connection");
            break;
        }
        else
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "recv() failed: " +
                                           std::string(strerror(errno)));
    }
}

void ConnectionHandler::_handleWrite(int fd)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Write event on fd: " + utils::toString(fd));

    if (_error_buffer.find(fd) != _error_buffer.end())
        _sendError(fd);
    else if (_write_buffer.find(fd) != _write_buffer.end())
        _sendNormal(fd);
    else
        weblog::Logger::log(weblog::DEBUG, "No write buffer found for fd: " +
                                               utils::toString(fd) +
                                               ", do nothing");
}

void ConnectionHandler::_sendNormal(int fd)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Write buffer found for fd: " + utils::toString(fd));

    std::map<int, std::string>::iterator it = _write_buffer.find(fd);
    int bytes_sent = send(fd, it->second.c_str(), it->second.size(), 0);

    if (bytes_sent < 0)
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "send() failed: " +
                                       std::string(strerror(errno)));
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
    }
}

void ConnectionHandler::_sendError(int fd)
{
    std::map<int, std::string>::iterator it = _error_buffer.find(fd);

    if (it == _error_buffer.end())
        closeConnection(fd, weblog::ERROR,
                        "Error buffer not found for fd: " +
                            utils::toString(fd));
    else
    {
        weblog::Logger::log(weblog::DEBUG, "Write buffer found for fd: " +
                                               utils::toString(fd));
        int bytes_sent = send(fd, it->second.c_str(), it->second.size(), 0);

        _error_buffer.erase(it);
        if (bytes_sent < 0)
        {
            closeConnection(fd, weblog::ERROR,
                            "send() failed: " + std::string(strerror(errno)));
        }
        else
        {
            closeConnection(fd, weblog::WARNING,
                            "Close the connection after handled error on fd: " +
                                utils::toString(fd));
        }
    }
}

// TODO: Consider implementing the keep-alive timeout in the processor,
// because we need the request headers to determine the connection type bool
// ConnectionHandler::_keepAlive(void)
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
