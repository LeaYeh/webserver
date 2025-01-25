#pragma once
#include "Config.hpp"
#include <cstring>
#include <ctime>
#include <string>

namespace webkernel
{

enum SessionMessageType
{
    SESSION_GET,
    SESSION_SET,
    SESSION_DELETE,
    SESSION_RESPONSE,
    SESSION_ERROR
};

struct SessionMessage
{
    SessionMessageType type;
    char session_id[64];
    size_t data_length;
    char data[4096];

    SessionMessage() { std::memset(this, 0, sizeof(SessionMessage)); }
};

struct SessionData
{
    std::string data;
    time_t create_time;
    time_t last_access;
    time_t expire_time;
    bool is_valid;

    SessionData() : is_valid(false) {}
};

struct SessionConfig
{
    std::string socket_path;
    size_t expire_seconds;
    size_t max_connections;

    SessionConfig()
    {
        const webconfig::ConfigGlobalBlock& config =
            webconfig::Config::instance()->global_block();

        socket_path = config.session_socket_path();
        expire_seconds = config.session_expire_seconds();
        max_connections = config.session_max_connections();
    }
};

} // namespace webkernel
