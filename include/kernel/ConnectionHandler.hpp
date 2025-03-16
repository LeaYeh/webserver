#pragma once

#include "HttpException.hpp"
#include "IHandler.hpp"
#include "Reactor.hpp"
#include "RequestProcessor.hpp"
#include "Singleton.hpp"
#include "VirtualHostManager.hpp"
#include "defines.hpp"
#include <ctime>
#include <map>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{

static const size_t BUFFER_SIZE = 4096;
class ConnectionHandler :
    public IHandler,
    public templates::Singleton<ConnectionHandler>
{
public:
    VirtualHostManager vhost_manager;

public:
    static ConnectionHandler* create_instance();

    void handle_event(int fd, uint32_t events);
    void close_connection(int fd, weblog::LogLevel level, std::string message);
    void prepare_write(int fd, const std::string& buffer);
    void prepare_error(int fd, const utils::HttpException& e);
    bool request_session_data(const std::string& sid, int client_fd);
    void recv_session_data();
    // bool get_session_data(const std::string& sid, std::string& out_data);
    bool set_session_data(const std::string& sid, const std::string& data);
    std::string get_session_data(const std::string& sid);
    bool has_session_data(const std::string& sid) const;

public:
    ~ConnectionHandler();

private:
    struct PendingSessionRequest
    {
        static const int MAX_RETRY = 3;
        static const int RETRY_INTERVAL = 5;
        static const int MAX_WAIT_TIME = 3;
        PendingSessionRequest(const std::string& sid, int client_fd) :
            session_id(sid),
            client_fd(client_fd),
            completed(false),
            request_time(time(0)),
            retry_count(0)
        {
        }
        std::string session_id;
        int client_fd;
        std::string out_data;
        bool completed;
        time_t request_time;
        int retry_count;
    };

private:
    typedef std::map<std::string /*sid*/, PendingSessionRequest*> PendingSessionRequestMap;
    RequestProcessor _processor;
    int _session_fd;
    // TODO: Use _read_buffer to handle extra data which out of a request
    // TODO: Consider to use pair instead of map
    std::map<int /*fd*/, std::string> _read_buffer;
    std::map<int /*fd*/, std::string> _write_buffer;
    std::map<int /*fd*/, std::string> _error_buffer;
    PendingSessionRequestMap _pending_session_requests;

private:
    void _handle_read(int fd);
    void _handle_read_eof(int fd);
    void _process_read_data(int fd, const char buffer[], ssize_t bytes_read);
    void _handle_write(int fd);
    void _send_normal(int fd);
    void _send_error(int fd);
    bool _is_buffer_full(const std::string& buffer) const;
    bool _init_buffer(std::string& buffer);
    void _connect_to_session_manager();

private:
    ConnectionHandler();
    ConnectionHandler(const ConnectionHandler& other);
    ConnectionHandler& operator=(const ConnectionHandler& other);
};

} // namespace webkernel
