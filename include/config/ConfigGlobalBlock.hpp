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

    unsigned int worker_processes(void) const;
    unsigned int worker_connections(void) const;

public:
    ConfigGlobalBlock();
    ConfigGlobalBlock(const ConfigGlobalBlock& other);
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);
    ~ConfigGlobalBlock();

private:
    unsigned int _worker_processes;
    unsigned int _worker_connections;

private:
    void _parse_config_directive(const std::string& line);
};

} // namespace webconfig
