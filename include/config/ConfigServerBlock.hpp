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

    std::string serverName(void) const;
    std::pair<std::string, std::string> listen(void) const;
    std::vector<std::pair<std::string, weblog::LogLevel> > errorLog(void) const;
    unsigned int keepAliveTimeout(void) const;
    std::vector<ConfigLocationBlock>& locationBlockList(void);

    std::string parse(std::ifstream& file_stream);
    void printConfig(void) const;

  private:
    std::string _server_name;
    std::pair<std::string, std::string> _listen;
    std::vector<std::pair<std::string, weblog::LogLevel> > _error_log;
    unsigned int _keep_alive_timeout;
    std::vector<ConfigLocationBlock> _location_block_list;

    void _parseConfigDirective(const std::string& line);
    std::pair<std::string, std::string>
    _parseListen(const std::string& line, const std::string& directive);
    std::pair<std::string, weblog::LogLevel>
    _parseErrorLog(const std::string& line, const std::string& directive);
    unsigned int _parseKeepAliveTimeout(const std::string& line,
                                        const std::string& directive);
};

} // namespace webconfig
