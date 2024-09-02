#pragma once

#include "AConfigParser.hpp"
#include "ConfigLocationBlock.hpp"
#include <fcntl.h>
#include <set>
#include <vector>

namespace webconfig
{

class ConfigServerBlock : public AConfigParser
{
  public:
    ConfigServerBlock();
    ~ConfigServerBlock();
    ConfigServerBlock(const ConfigServerBlock& other);
    ConfigServerBlock& operator=(const ConfigServerBlock& other);

    std::string server_name(void) const;
    std::pair<std::string, std::string> listen(void) const;
    std::vector<std::pair<std::string, weblog::LogLevel> > error_log(void) const;
    unsigned int keep_alive_timeout(void) const;
    std::vector<ConfigLocationBlock>& location_block_list(void);

    std::string parse(std::ifstream& file_stream);
    void print_config(void) const;

  private:
    std::string _server_name;
    std::pair<std::string, std::string> _listen;
    std::vector<std::pair<std::string, weblog::LogLevel> > _error_log;
    unsigned int _keep_alive_timeout;
    std::vector<ConfigLocationBlock> _location_block_list;

    void _parse_config_directive(const std::string& line);
    std::pair<std::string, std::string>
    _parse_listen(const std::string& line, const std::string& directive);
    std::pair<std::string, weblog::LogLevel>
    _parse_error_log(const std::string& line, const std::string& directive);
    unsigned int _parse_keep_alive_timeout(const std::string& line,
                                           const std::string& directive);
};

} // namespace webconfig
