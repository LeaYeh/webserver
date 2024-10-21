#pragma once

#include "ConfigGlobalBlock.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "defines.hpp"
#include "Singleton.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace webconfig
{

class Config : public templates::Singleton<Config, std::string>
{
  public:
    static Config* createInstance();
    static Config* createInstance(const std::string& filename);
    
    ~Config();

    std::string filename(void) const;
    const ConfigGlobalBlock& globalBlock(void) const;
    const ConfigHttpBlock& httpBlock(void) const;
    const std::vector<ConfigServerBlock>& serverBlockList(void) const;
    std::vector<ConfigServerBlock>& serverBlockList(void);

    void printConfig(void) const;

  private:
    Config();
    Config(const std::string& filename);
    Config(const Config& other);
    Config& operator=(const Config& other);
    ConfigBlockLevel _current_block_level;
    std::string _filename;
    std::ifstream _file_stream;
    ConfigGlobalBlock _global_block;
    ConfigHttpBlock _http_block;
    std::vector<ConfigServerBlock> _server_block_list;

    bool _setBlockLevel(const std::string& line);
    void _parse(void);
};

} // namespace webconfig
