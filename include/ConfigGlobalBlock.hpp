#pragma once

#include "AConfigParser.hpp"
#include <string>

namespace webconfig
{

class ConfigGlobalBlock : public AConfigParser
{
  public:
    unsigned int worker_processes;
    unsigned int worker_connections;

    ConfigGlobalBlock();
    ~ConfigGlobalBlock();

    void parse(std::ifstream &file_stream);

  private:
    ConfigGlobalBlock(const ConfigGlobalBlock& other);
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);
};

} // namespace webconfig
