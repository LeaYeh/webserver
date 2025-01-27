#include "SessionManager.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include "session_types.hpp"
#include "utils.hpp"
#include <cerrno>
#include <cstdio>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

namespace webkernel
{

SessionManager::SessionManager(const SessionConfig& config) : _config(config)
{
    _server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_server_fd < 0) {
        throw std::runtime_error("Failed to create socket for session manager: "
                                 + utils::to_string(strerror(errno)));
    }
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    const std::string socket_path = "/tmp/session.sock";
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
    std::remove(socket_path.c_str());

    if (bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(_server_fd);
        throw std::runtime_error("Failed to bind session socket: "
                                 + utils::to_string(strerror(errno)));
    }

    if (listen(_server_fd, SOMAXCONN) < 0) {
        close(_server_fd);
        throw std::runtime_error("Failed to listen on session socket: "
                                 + utils::to_string(strerror(errno)));
    }

    LOG(weblog::DEBUG,
        std::string("Session manager listening on socket: ")
            + utils::to_string(_server_fd));
}

SessionManager::SessionManager(const SessionManager& other)
{
    _server_fd = other._server_fd;
    _sessions = other._sessions;
}

SessionManager& SessionManager::operator=(const SessionManager& other)
{
    if (this != &other) {
        _server_fd = other._server_fd;
        _sessions = other._sessions;
    }
    return (*this);
}

SessionManager::~SessionManager()
{
    close(_server_fd);
}

void SessionManager::handle_event(int fd, uint32_t events)
{
    if (!(events & EPOLLIN)) {
        throw std::runtime_error("SessionManager: Unexpected event: "
                                 + utils::to_string(events));
    }
    if (_is_listening_socket(fd)) {
        _handle_new_connection();
    }
    else if (_is_client_socket(fd)) {
        _handle_session_request(fd);
    }
    else {
        throw std::runtime_error("SessionManager: Unknown fd: "
                                 + utils::to_string(fd));
    }
}

int SessionManager::get_fd() const
{
    return (_server_fd);
}

bool SessionManager::_is_client_socket(int fd) const
{
    return (_client_fds.find(fd) != _client_fds.end());
}

bool SessionManager::_is_listening_socket(int fd) const
{
    return (fd == _server_fd);
}

void SessionManager::_handle_new_connection()
{
    struct sockaddr_un addr;
    socklen_t addr_len = sizeof(addr);
    int client_fd = accept(_server_fd, (struct sockaddr*)&addr, &addr_len);

    if (client_fd < 0) {
        throw std::runtime_error("Failed to accept new session connection: "
                                 + utils::to_string(strerror(errno)));
    }
    if (static_cast<unsigned int>(_client_fds.size())
        >= _config.max_connections) {
        LOG(weblog::DEBUG,
            "Session connection limit reached, closing: "
                + utils::to_string(client_fd));
        close(client_fd);
        return;
    }
    LOG(weblog::DEBUG,
        "SessionManager: Accepted new session connection: "
            + utils::to_string(client_fd));
    _client_fds.insert(client_fd);
    Reactor::instance()->register_handler(client_fd, this, EPOLLIN);
}

void SessionManager::_handle_session_request(int fd)
{
    SessionMessage msg;
    int bytes = recv(fd, &msg, sizeof(msg), 0);

    if (bytes < 0) {
        throw std::runtime_error("Failed to receive session message: "
                                 + utils::to_string(strerror(errno)));
    }
    if (bytes == 0) {
        LOG(weblog::DEBUG,
            "Session connection closed: " + utils::to_string(fd));
        _client_fds.erase(fd);
        Reactor::instance()->remove_handler(fd);
        close(fd);
        return;
    }
    if (bytes != sizeof(msg)) {
        throw std::runtime_error("Invalid session message size: "
                                 + utils::to_string(bytes));
    }
    switch (msg.type) {
    case SESSION_GET:
        _handle_session_get(fd, msg);
        break;
    case SESSION_SET:
        _handle_session_set(fd, msg);
        break;
    case SESSION_DELETE:
        _handle_session_delete(fd, msg);
        break;
    default:
        _send_error(fd, "Invalid session message type");
        break;
    }
}

void SessionManager::_handle_session_set(int fd, const SessionMessage& msg)
{
    SessionData& session = _sessions[msg.session_id];

    session.data = std::string(msg.data, msg.data_length);
    if (!session.is_valid) {
        session.create_time = time(0);
        session.is_valid = true;
    }
    session.expire_time = time(0) + _config.expire_seconds;
    session.last_access = time(0);
    _send_resp(fd, SESSION_RESPONSE, "Session data updated");
}

void SessionManager::_handle_session_get(int fd, const SessionMessage& msg)
{
    std::map<std::string, SessionData>::iterator it =
        _sessions.find(msg.session_id);

    if (it == _sessions.end() || !it->second.is_valid) {
        _send_error(fd, "Session not found");
        return;
    }
    SessionData& session = it->second;

    if (session.expire_time < time(0)) {
        _send_error(fd, "Session expired");
        session.is_valid = false;
        return;
    }
    session.last_access = time(0);
    _send_resp(fd, SESSION_RESPONSE, session.data);
}

void SessionManager::_handle_session_delete(int fd, const SessionMessage& msg)
{
    _sessions.erase(msg.session_id);
    _send_resp(fd, SESSION_RESPONSE, "Session deleted");
}

void SessionManager::_send_resp(int fd,
                                SessionMessageType type,
                                const std::string& data)
{
    SessionMessage msg;

    msg.type = type;
    std::strncpy(msg.session_id, "server", sizeof(msg.session_id));
    msg.data_length = data.length();
    std::strncpy(msg.data, data.c_str(), sizeof(msg.data));
    if (send(fd, &msg, sizeof(msg), 0) < 0) {
        throw std::runtime_error("Failed to send session response: "
                                 + utils::to_string(strerror(errno)));
    }
}

void SessionManager::_send_error(int fd, const std::string& msg)
{
    _send_resp(fd, SESSION_ERROR, msg);
}

void SessionManager::cleanup_expired_sessions()
{
    time_t now = time(0);

    for (std::map<std::string, SessionData>::iterator it = _sessions.begin();
         it != _sessions.end();
         ++it) {
        if (it->second.expire_time < now) {
            _sessions.erase(it);
        }
    }
}

} // namespace webkernel
