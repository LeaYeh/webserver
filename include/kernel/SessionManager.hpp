#pragma once
#include "IHandler.hpp"
#include "session_types.hpp"
#include <map>
#include <set>

namespace webkernel
{
class SessionManager : public IHandler
{
public:
    void handle_event(int fd, uint32_t events);
    int get_fd() const;
    void cleanup_expired_sessions();

public:
    SessionManager(const SessionConfig& config);
    SessionManager(const SessionManager& other);
    SessionManager& operator=(const SessionManager& other);
    ~SessionManager();

private:
    SessionConfig _config;
    int _server_fd;
    std::map<std::string /* SID */, SessionData> _sessions;
    std::set<int /* fd */> _client_fds;

private:
    bool _is_client_socket(int fd) const;
    bool _is_listening_socket(int fd) const;
    void _handle_new_connection();
    void _handle_session_request(int fd);
    void _handle_session_set(int fd, const SessionMessage& msg);
    void _handle_session_get(int fd, const SessionMessage& msg);
    void _handle_session_delete(int fd, const SessionMessage& msg);
    void _send_resp(int fd, SessionMessageType type, const std::string& data);
    void _send_error(int fd, const std::string& msg);

private:
    SessionManager();
};

} // namespace webkernel
