#pragma once

#include "ConfigGlobalBlock.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "defines.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace webconfig
{

class ServerConfig
{
  public:
    ServerConfig(const std::string& filename);
    ~ServerConfig();

    void parse(void);
    unsigned int worker_processes(void) const;
    unsigned int worker_connections(void) const;

  private:
    ServerConfig();
    ServerConfig(const ServerConfig& other);
    ServerConfig& operator=(const ServerConfig& other);

    std::string _filename;
    std::ifstream _file_stream;
    ConfigGlobalBlock _global_block;
    ConfigHttpBlock _http_block;
    std::vector<ConfigServerBlock> _server_block_list;

    bool _set_block_level(const std::string& line);
    ConfigBlockLevel _current_block_level;
};

} // namespace webconfig
