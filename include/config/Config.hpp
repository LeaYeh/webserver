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

class Config
{
  public:
    Config(const std::string& filename);
    ~Config();

    void parse(void);
    void print_config(void) const;

    std::string filename(void) const;
    ConfigGlobalBlock& global_block(void);
    ConfigHttpBlock& http_block(void);
    std::vector<ConfigServerBlock>& server_block_list(void);

  private:
    Config();
    Config(const Config& other);
    Config& operator=(const Config& other);

    ConfigBlockLevel _current_block_level;
    std::string _filename;
    std::ifstream _file_stream;
    ConfigGlobalBlock _global_block;
    ConfigHttpBlock _http_block;
    std::vector<ConfigServerBlock> _server_block_list;

    bool _set_block_level(const std::string& line);
};

} // namespace webconfig
