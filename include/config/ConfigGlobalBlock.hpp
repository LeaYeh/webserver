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
    ConfigGlobalBlock(const ConfigGlobalBlock& other);
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);
    ~ConfigGlobalBlock();

    void printConfig(void) const;
    std::string parse(std::ifstream& file_stream);

    unsigned int workerProcesses(void) const;
    unsigned int workerConnections(void) const;

  private:
    unsigned int _worker_processes;
    unsigned int _worker_connections;

    void _parseConfigDirective(const std::string& line);
};

} // namespace webconfig
