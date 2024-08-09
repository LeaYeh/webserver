#pragma once

#include "AConfigParser.hpp"
#include "ConfigLocationBlock.hpp"
#include <vector>

namespace webconfig
{

class ConfigServerBlock : public AConfigParser
{
  public:
    ConfigServerBlock();
    ~ConfigServerBlock();

    void parse(std::ifstream& file_stream);

  private:
    ConfigServerBlock(const ConfigServerBlock& other);
    ConfigServerBlock& operator=(const ConfigServerBlock& other);

    std::string _server_name;
    std::pair<std::string, unsigned int> _listen;
    std::vector<std::pair<std::string, weblog::LogLevel> > _error_log;
    unsigned int _keepalive_timeout;
    unsigned int _keepalive_requests;
    std::vector<ConfigLocationBlock> _location_block;
};

} // namespace webconfig
