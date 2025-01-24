#pragma once
#include "AConfigParser.hpp"
#include <string>

namespace webconfig
{

class ConfigGlobalBlock : public AConfigParser
{
public:
    void print_config(void) const;
    std::string parse(std::ifstream& file_stream);

    size_t worker_processes(void) const;
    size_t worker_connections(void) const;
    const std::string& session_socket_path(void) const;
    size_t session_expire_seconds(void) const;
    size_t session_max_connections(void) const;

public:
    ConfigGlobalBlock();
    ConfigGlobalBlock(const ConfigGlobalBlock& other);
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);
    ~ConfigGlobalBlock();

private:
    size_t _worker_processes;
    size_t _worker_connections;
    std::string _session_socket_path;
    size_t _session_expire_seconds;
    size_t _session_max_connections;

private:
    void _parse_config_directive(const std::string& line);
};

} // namespace webconfig
