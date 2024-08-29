#include "ConnectionHandler.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <ctime>

namespace webkernel
{

ConnectionHandler::ConnectionHandler()
    : _conn_fd(-1), _start_time(time(0)), _reactor(NULL), _server_id(-1)
{
}

ConnectionHandler::ConnectionHandler(int conn_fd, Reactor* reactor,
                                     webconfig::Config* config, int server_id)
    : _conn_fd(conn_fd), _start_time(time(0)), _reactor(reactor),
      _server_id(server_id)
{
    weblog::logger.log(weblog::DEBUG,
                       "ConnectionHandler created with server_id: " +
                           utils::to_string(server_id));
    _http_config = config->http_block();
    _server_config = config->server_block_list()[server_id];
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler& other)
    : _conn_fd(other._conn_fd), _start_time(other._start_time),
      _reactor(other._reactor), _server_id(other._server_id),
      _http_config(other._http_config), _server_config(other._server_config)
{
}

ConnectionHandler& ConnectionHandler::operator=(const ConnectionHandler& other)
{
    if (this != &other)
    {
        _conn_fd = other._conn_fd;
        _start_time = other._start_time;
        _reactor = other._reactor;
        _server_id = other._server_id;
        _http_config = other._http_config;
        _server_config = other._server_config;
    }
    return (*this);
}

ConnectionHandler::~ConnectionHandler()
{
    weblog::logger.log(weblog::DEBUG,
                       "ConnectionHandler destroyed with server_id: " +
                           utils::to_string(_server_id));
    _handle_close(_conn_fd, weblog::INFO, "Connection closed by server");
}

void ConnectionHandler::handle_event(int fd, uint32_t events)
{
    if (events & EPOLLIN)
        _handle_read(fd);
    else if (events & EPOLLOUT)
        _handle_write(fd);
    else if (events & EPOLLHUP)
        _handle_close(fd, weblog::INFO,
                      "Connection closed by client " + utils::to_string(fd));
    else if (events & EPOLLERR)
        _handle_close(fd, weblog::ERROR,
                      "EPOLLERR on fd: " + utils::to_string(fd));
    else
        _handle_close(fd, weblog::ERROR,
                      "Handler got unknown event: " + utils::to_string(events));
}

void ConnectionHandler::_handle_read(int fd)
{
    char buffer[CHUNKED_SIZE];

    while (true)
    {
        ssize_t bytes_read = read(fd, buffer, CHUNKED_SIZE);
        if (bytes_read == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                _handle_close(fd, weblog::ERROR,
                              "read() failed: " + std::string(strerror(errno)));
                return;
            }
        }
        else if (bytes_read == 0)
        {
            _handle_close(fd, weblog::INFO, "Connection closed by client");
            return;
        }
        else
        {
            weblog::logger.log(weblog::DEBUG,
                               "Read " + utils::to_string(bytes_read) +
                                   " bytes from fd: " + utils::to_string(fd));
            _request_parser.update_state(buffer, bytes_read);
            if (_request_parser.is_complete())
            {
                weblog::logger.log(weblog::INFO, "Request is complete");
                // Process the request
                break;
            }
        }
    }
    if (!_keep_alive())
        _handle_close(fd, weblog::INFO, "Keep-alive timeout or close request");
}

void ConnectionHandler::_handle_write(int fd)
{
    (void)fd;
}

void ConnectionHandler::_handle_close(int fd, weblog::LogLevel level,
                                      std::string message)
{
    weblog::logger.log(level, message);
    utils::safe_close(fd);
    _reactor->remove_handler(fd);
}

bool ConnectionHandler::_keep_alive(void)
{
    time_t now = time(0);
    double elapsed = difftime(now, _start_time);

    if (_request_parser.header_analyzer().content_type() == webshell::CLOSE)
        return (false);
    else if (elapsed > _server_config.keep_alive_timeout())
        return (false);

    return (true);
}

} // namespace webkernel
