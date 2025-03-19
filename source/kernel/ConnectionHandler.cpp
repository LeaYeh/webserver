#include "ConnectionHandler.hpp"
#include "CgiExecutor.hpp"
#include "Config.hpp"
#include "ConfigGlobalBlock.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "debugUtils.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "session_types.hpp"
#include "utils.hpp"
#include <sys/epoll.h>
#include <sys/un.h>
#include <unistd.h>

namespace webkernel
{

ConnectionHandler::ConnectionHandler() : _processor(this), _session_fd(-1) {}

ConnectionHandler::~ConnectionHandler()
{
    LOG(weblog::DEBUG, "ConnectionHandler destroyed");
    close(_session_fd);
    // TODO: Check the conn_fd is closed by the reactor
    // _handleClose(_conn_fd, weblog::INFO, "Connection closed by server");
}

ConnectionHandler* ConnectionHandler::create_instance()
{
    return new ConnectionHandler();
}

void ConnectionHandler::handle_event(int fd, uint32_t events)
{
    // TODO: Consider to make connection to session manager in the constructor
    // if (_session_fd == -1) {
    //     _connect_to_session_manager();
    // }
    LOG(weblog::DEBUG,
        "ConnectionHandler::handle_event() on fd: " + utils::to_string(fd)
            + " with events: " + explain_epoll_event(events));
    if (events & EPOLLIN) {
        _handle_read(fd);
    }
    else if (events & EPOLLHUP) {
        close_connection(fd, weblog::INFO, "Connection closed by client");
    }
    else if (events & EPOLLERR) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "EPOLLERR event on fd: "
                                       + utils::to_string(fd));
    }
    else if (events & EPOLLOUT) {
        _handle_write(fd);
    }
    else {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Unknown event on fd: "
                                       + utils::to_string(fd));
    }
}

void ConnectionHandler::close_connection(int fd,
                                         weblog::LogLevel level,
                                         std::string message)
{
    LOG(level, message + " on conn_fd: " + utils::to_string(fd));
    // if (CgiExecutor::instance()->handler_exists(fd)) {
    //     LOG(weblog::DEBUG,
    //         "Remove cgi handler on fd: " + utils::to_string(fd));
    //     CgiExecutor::instance()->remove_handler(fd);
    // }
    // TODO: maybe this is the reason why we failed the cgi siege test
    // TODO: Need to remove the handler from the reactor in different way
    Reactor::instance()->remove_handler(fd);
    _processor.remove_state(fd);
    _processor.remove_analyzer(fd);
    _read_buffer.erase(fd);
    _write_buffer.erase(fd);
}

void ConnectionHandler::prepare_write(int fd, const std::string& buffer)
{
    LOG(weblog::DEBUG,
        "Prepare to write " + utils::to_string(buffer.size())
            + " bytes to fd: " + utils::to_string(fd));
    if (_write_buffer.find(fd) == _write_buffer.end()
        && !_init_buffer(_write_buffer[fd])) {
        close_connection(fd, weblog::ERROR, "Write buffer allocation failed");
    }

    // TODO: Check why who call the prepare_write even though the fd is not
    // exist any more.
    _write_buffer[fd].append(buffer);
    Reactor::instance()->modify_handler(fd, EPOLLOUT, 0);
}

void ConnectionHandler::prepare_error(int fd, const utils::HttpException& e)
{
    LOG(weblog::DEBUG,
        "Prepare to write error response to fd: " + utils::to_string(fd));
    if (_error_buffer.find(fd) == _error_buffer.end()
        && !_init_buffer(_error_buffer[fd])) {
        close_connection(fd, weblog::ERROR, "Error buffer allocation failed");
    }
    webshell::Response err_response = webshell::ResponseBuilder::error(
        e.status_code(), e.reasonDetail(), e.contentType());

    LOG(weblog::WARNING, "Error response: \n" + err_response.serialize());

    _error_buffer[fd].append(err_response.serialize());
    Reactor::instance()->modify_handler(fd, EPOLLOUT, EPOLLIN);
    _processor.set_state(fd, ERROR);
}

bool ConnectionHandler::set_session_data(const std::string& sid,
                                         const std::string& data)
{
    SessionMessage msg;

    msg.type = SESSION_SET;
    std::strncpy(msg.session_id, sid.c_str(), sizeof(msg.session_id));
    msg.data_length = data.length();
    std::strncpy(msg.data, data.c_str(), sizeof(msg.data));
    int bytes = send(_session_fd, &msg, sizeof(msg), 0);

    if (bytes != sizeof(msg)) {
        LOG(weblog::ERROR,
            "Send session message failed: " + utils::to_string(bytes));
        return (false);
    }
    Reactor::instance()->modify_handler(_session_fd, EPOLLIN, 0);
    return (true);
}

/*
ConnectionHandler:
    1. the buffer is full, the request analyzer needs to comsume the buffer and
update the state
    2. EOF is reached, the current state of the request analyzer is the final
state
*/
void ConnectionHandler::_handle_read(int fd)
{
    if (_read_buffer.find(fd) == _read_buffer.end()
        && !_init_buffer(_read_buffer[fd])) {
        close_connection(fd, weblog::ERROR, "Read buffer allocation failed");
        return;
    }
    if (_is_buffer_full(_read_buffer[fd])) {
        LOG(weblog::DEBUG,
            "Read buffer is full, need to consume the buffer first.");
        Reactor::instance()->modify_handler(fd, EPOLLOUT, EPOLLIN);
        return;
    }
    char buffer[CHUNKED_SIZE];
    ssize_t bytes_read = recv(fd, buffer, CHUNKED_SIZE, 0);

    if (bytes_read < 0) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "recv() failed.");
    }
    else if (bytes_read == 0) {
        _handle_read_eof(fd);
    }
    else {
        _process_read_data(fd, buffer, bytes_read);
    }
}

void ConnectionHandler::_handle_read_eof(int fd)
{
    if (_write_buffer.find(fd) == _write_buffer.end()
        || _write_buffer[fd].empty()) {
        close_connection(
            fd, weblog::INFO, "Read 0 bytes, client closing connection");
    }
    else {
        close_connection(fd,
                         weblog::WARNING,
                         "Read 0 bytes, client closing connection, "
                         "but there is data to write");
    }
}

void ConnectionHandler::_process_read_data(int fd,
                                           const char buffer[],
                                           ssize_t bytes_read)
{
    LOG(weblog::DEBUG,
        "Read " + utils::to_string(bytes_read)
            + " bytes from fd: " + utils::to_string(fd));
    LOG(weblog::DEBUG,
        "Read content: \n"
            + utils::replaceCRLF(std::string(buffer, bytes_read)));
    _read_buffer[fd].append(buffer, bytes_read);
    EventProcessingState process_state = _processor.state(fd);
    if (!(process_state & PROCESSING)) {
        _processor.analyze(fd, _read_buffer[fd]);
    }
    else {
        _processor.process(fd);
    }
}

void ConnectionHandler::_handle_write(int fd)
{
    LOG(weblog::DEBUG,
        "Write event on fd: " + utils::to_string(fd) + " with state: "
            + explain_event_processing_state(_processor.state(fd)));

    EventProcessingState process_state = _processor.state(fd);
    if (process_state & WAITING_CGI) {
        // _send_normal(fd);
        process_state = CONSUME_BODY;
        _processor.set_state(fd, CONSUME_BODY);
    }
    if (process_state & ERROR) {
        // if (_error_buffer.find(fd) != _error_buffer.end()) {
        _send_error(fd);
    }
    else if (process_state & CONSUME_BODY) {
        _processor.process(fd);
    }
    else if (process_state & COMPELETED) {
        _send_normal(fd);
        // Reset adn ready to analyze the next request
        _processor.remove_analyzer(fd);
        // Reactor::instance()->modify_handler(fd, EPOLLIN, EPOLLOUT);
        if (_processor.need_to_close(fd)) {
            close_connection(fd, weblog::INFO, "Connection closed by server");
        }
    }
    else if (process_state & HANDLE_CHUNKED) {
        _send_normal(fd);
        _processor.process(fd);
    }
    else {
        LOG(weblog::ERROR,
            "Unknown state " + utils::to_string(process_state)
                + " on fd: " + utils::to_string(fd));
    }
}

void ConnectionHandler::_send_normal(int fd)
{
    if (_write_buffer.find(fd) == _write_buffer.end()) {
        LOG(weblog::DEBUG,
            "No write buffer found for fd: " + utils::to_string(fd)
                + ", do nothing");
        return;
    }
    if (_write_buffer[fd].empty()) {
        LOG(weblog::DEBUG,
            "Empty write buffer found for fd: " + utils::to_string(fd)
                + ", do nothing");
        return;
    }
    LOG(weblog::DEBUG,
        "Write content: \n" + utils::replaceCRLF(_write_buffer[fd]));
    int bytes_sent =
        send(fd, _write_buffer[fd].c_str(), _write_buffer[fd].size(), 0);

    if (CgiExecutor::instance()->handler_exists(fd)) {
        LOG(weblog::DEBUG,
            "Remove cgi handler on pipe fd: " + utils::to_string(fd));
        CgiExecutor::instance()->remove_handler(fd);
    }
    Reactor::instance()->modify_handler(fd, EPOLLIN, EPOLLOUT);
    if (bytes_sent < 0) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "send() failed.");
    }
    else if (bytes_sent == 0) {
        LOG(weblog::INFO, "Write 0 bytes, client closing connection");
        close_connection(
            fd, weblog::INFO, "Write 0 bytes, client closing connection");
    }
    else {
        LOG(weblog::DEBUG,
            "Sent " + utils::to_string(bytes_sent)
                + " bytes to fd: " + utils::to_string(fd));
    }
    _write_buffer.erase(fd);
}

void ConnectionHandler::_send_error(int fd)
{
    std::map<int, std::string>::iterator it = _error_buffer.find(fd);

    if (it == _error_buffer.end()) {
        LOG(weblog::WARNING,
            "No error buffer found for fd: " + utils::to_string(fd)
                + ", do nothing");
        return;
    }
    else {
        LOG(weblog::DEBUG,
            "Error buffer found for fd: " + utils::to_string(fd));
        int bytes_sent = send(fd, it->second.c_str(), it->second.size(), 0);

        _error_buffer.erase(it);
        if (bytes_sent < 0) {
            close_connection(fd,
                             weblog::ERROR,
                             "send() failed.");
        }
        else if (bytes_sent == 0) {
            close_connection(fd,
                             weblog::WARNING,
                             "Write 0 bytes, client closing connection");
        }
        else {
            close_connection(fd,
                             weblog::WARNING,
                             "Close the connection after handled error on fd: "
                                 + utils::to_string(fd));
        }
    }
}

bool ConnectionHandler::_is_buffer_full(const std::string& buffer) const
{
    return (buffer.size() + CHUNKED_SIZE > BUFFER_SIZE);
}

bool ConnectionHandler::_init_buffer(std::string& buffer)
{
    buffer.clear();
    try {
        buffer.reserve(BUFFER_SIZE);
    }
    catch (const std::bad_alloc& e) {
        return (false);
    }
    if (buffer.capacity() < BUFFER_SIZE) {
        return (false);
    }
    return (true);
}

void ConnectionHandler::_connect_to_session_manager()
{
    const std::string& socket_path =
        webconfig::Config::instance()->global_block().session_socket_path();
    struct sockaddr_un addr;

    if ((_session_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("Failed to create session socket.");
    }
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path));
    if (connect(_session_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(_session_fd);
        throw std::runtime_error("Failed to connect to session manager.");
    }
    LOG(weblog::DEBUG,
        "Connected to session manager on fd: " + utils::to_string(_session_fd));
}

} // namespace webkernel
