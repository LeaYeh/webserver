#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "ConfigGlobalBlock.hpp"
#include "ConfigServerBlock.hpp"

namespace webconfig
{

class ServerConfig
{
  public:
    ServerConfig(const std::string& filename);
    ~ServerConfig();

    void parse(const std::string& line);

  private:
    ServerConfig();
    ServerConfig(const ServerConfig& other);
    ServerConfig& operator=(const ServerConfig& other);

    std::string _filename;
    std::ifstream _file_stream;
    ConfigGlobalBlock _global_block;
    std::vector<ConfigServerBlock> _server_block_list;
};

} // namespace webconfig
