#pragma once

#include "ConfigGlobalBlock.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "Singleton.hpp"
#include "defines.hpp"
#include <string>
#include <vector>

namespace webconfig
{

class Config : public templates::Singleton<Config, std::string>
{
public:
    static Config* create_instance();
    static Config* create_instance(const std::string& filename);

    std::string filename(void) const;
    const ConfigGlobalBlock& global_block(void) const;
    const ConfigHttpBlock& http_block(void) const;
    const std::vector<ConfigServerBlock>& server_block_list(void) const;
    std::vector<ConfigServerBlock>& server_block_list(void);

    void print_config(void) const;
    bool validate(void) const;

public:
    ~Config();

private:
    ConfigBlockLevel _current_block_level;
    std::string _filename;
    std::ifstream _file_stream;
    ConfigGlobalBlock _global_block;
    ConfigHttpBlock _http_block;
    std::vector<ConfigServerBlock> _server_block_list;

private:
    bool _set_block_level(const std::string& line);
    void _parse(void);

private:
    Config();
    Config(const std::string& filename);
    Config(const Config& other);
    Config& operator=(const Config& other);
};

} // namespace webconfig
