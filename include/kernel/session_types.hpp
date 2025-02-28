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

    SessionMessage() : type(SESSION_ERROR), data_length(0)
    {
        std::memset(session_id, 0, sizeof(session_id));
        std::memset(data, 0, sizeof(data));
    }

    void set_data(const std::string& data)
    {
        data_length = data.length();
        std::strncpy(this->data, data.c_str(), sizeof(this->data));
    }
    void set_id(const std::string& id)
    {
        std::strncpy(session_id, id.c_str(), sizeof(session_id));
    }

    std::string get_data() const { return (std::string(data, data_length)); }

    std::string get_id() const { return (std::string(session_id)); }
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
