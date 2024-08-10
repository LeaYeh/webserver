#pragma once

#include "AConfigParser.hpp"
#include <set>
#include <string>

namespace webconfig
{

class ConfigGlobalBlock : public AConfigParser
{
  public:

    ConfigGlobalBlock();
    ~ConfigGlobalBlock();

    std::string parse(std::ifstream &file_stream);

    unsigned int worker_processes;
    unsigned int worker_connections;

  private:
    ConfigGlobalBlock(const ConfigGlobalBlock& other);
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);

    void _parse_config_directive(const std::string& line);
};

} // namespace webconfig
