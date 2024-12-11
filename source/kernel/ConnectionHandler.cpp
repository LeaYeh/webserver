#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "debugUtils.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <sys/epoll.h>

namespace webkernel
{

ConnectionHandler::ConnectionHandler(Reactor* reactor)
    : _reactor(reactor), _processor(this)
{
}

ConnectionHandler::~ConnectionHandler()
{
    weblog::Logger::log(weblog::DEBUG, "ConnectionHandler destroyed");
    // TODO: Check the conn_fd is closed by the reactor
    // _handleClose(_conn_fd, weblog::INFO, "Connection closed by server");
}

Reactor* ConnectionHandler::reactor(void) const
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
    if (_write_buffer.find(fd) == _write_buffer.end())
        _write_buffer[fd].reserve(BUFFER_SIZE);

    _write_buffer[fd].append(buffer);
    _reactor->modifyHandler(fd, EPOLLOUT, 0);
}

void ConnectionHandler::prepareError(int fd, const utils::HttpException& e)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Prepare to write error response to fd: " +
                            utils::toString(fd));
    if (_error_buffer.find(fd) == _error_buffer.end())
        _error_buffer[fd].reserve(BUFFER_SIZE);
    // TODO: Here might occour OOM, need to consider closing the connection
    // directly. ref: PR#65
    webshell::Response err_response =
        webshell::ResponseBuilder::buildErrorResponse(
            e.statusCode(), e.reasonDetail(), e.contentType());

    weblog::Logger::log(weblog::WARNING,
                        "Error response: \n" + err_response.serialize());
    _error_buffer[fd].append(err_response.serialize());

    _reactor->modifyHandler(fd, EPOLLOUT, EPOLLIN);
    _processor.setState(fd, ERROR);
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
    if (_read_buffer.find(fd) == _read_buffer.end())
        _read_buffer[fd].reserve(BUFFER_SIZE);
    if (_is_buffer_full(_read_buffer[fd]))
    {
        weblog::Logger::log(
            weblog::DEBUG,
            "Read buffer is full, need to consume the buffer first.");
        _reactor->modifyHandler(fd, EPOLLOUT, EPOLLIN);
        return;
    }
    char buffer[CHUNKED_SIZE];
    ssize_t bytes_read = recv(fd, buffer, CHUNKED_SIZE, 0);

    if (bytes_read < 0)
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "recv() failed: " +
                                       std::string(strerror(errno)));
    else if (bytes_read == 0)
    {
        if (_write_buffer.find(fd) == _write_buffer.end() ||
            _write_buffer[fd].empty())
            closeConnection(fd, weblog::INFO,
                            "Read 0 bytes, client closing connection");
        else
            closeConnection(fd, weblog::WARNING,
                            "Read 0 bytes, client closing connection, but "
                            "there is data to write");
    }
    else
    {
        weblog::Logger::log(weblog::DEBUG,
                            "Read content: \n" + utils::replaceCRLF(std::string(
                                                     buffer, bytes_read)));
        _read_buffer[fd].append(std::string(buffer, bytes_read));
        _processor.analyze(fd, _read_buffer[fd]);
    }
}

void ConnectionHandler::_handleWrite(int fd)
{
    weblog::Logger::log(weblog::DEBUG,
                        "Write event on fd: " + utils::toString(fd) +
                            " with state: " +
                            explainEventProcessingState(_processor.state(fd)));

    const EventProcessingState& process_state = _processor.state(fd);
    if (process_state & ERROR)
        _sendError(fd);
    else if (process_state & COMPELETED)
    {
        _sendNormal(fd);
        _reactor->modifyHandler(fd, EPOLLIN, EPOLLOUT);
    }
    else if (process_state & HANDLE_CHUNKED)
    {
        _sendNormal(fd);
        _processor.process(fd);
    }
    else
    {
        weblog::Logger::log(weblog::ERROR,
                            "Unknown state " + utils::toString(process_state) +
                                " on fd: " + utils::toString(fd));
    }
}

void ConnectionHandler::_sendNormal(int fd)
{
    std::map<int, std::string>::iterator it = _write_buffer.find(fd);

    if (it == _write_buffer.end())
    {
        weblog::Logger::log(weblog::WARNING, "No write buffer found for fd: " +
                                                 utils::toString(fd) +
                                                 ", do nothing");
        return;
    }

    weblog::Logger::log(weblog::DEBUG,
                        "Write content: \n" + utils::replaceCRLF(it->second));
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
    }
    _write_buffer.erase(it);
}

void ConnectionHandler::_sendError(int fd)
{
    std::map<int, std::string>::iterator it = _error_buffer.find(fd);

    if (it == _error_buffer.end())
    {
        weblog::Logger::log(weblog::WARNING, "No error buffer found for fd: " +
                                                 utils::toString(fd) +
                                                 ", do nothing");
        return;
    }
    else
    {
        weblog::Logger::log(weblog::DEBUG, "Error buffer found for fd: " +
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

bool ConnectionHandler::_is_buffer_full(const std::string& buffer) const
{
    return (buffer.size() + CHUNKED_SIZE > BUFFER_SIZE);
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
