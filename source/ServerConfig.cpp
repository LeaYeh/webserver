#include "ServerConfig.hpp"
#include "defines.hpp"
#include <iostream>

namespace webconfig
{

ServerConfig::ServerConfig(const std::string& filename)
    : _current_block_level(GLOBAL), _filename(filename), _global_block(),
      _http_block(), _server_block_list()
{
    _file_stream.open(filename.c_str(), std::ifstream::in);
    if (!_file_stream.good() || !_file_stream.is_open())
        throw std::invalid_argument("Error opening file: " + filename);
}

ServerConfig::~ServerConfig()
{
    _file_stream.close();
}

std::string ServerConfig::filename(void) const
{
    return (_filename);
}

ConfigGlobalBlock& ServerConfig::global_block(void)
{
    return (_global_block);
}

ConfigHttpBlock& ServerConfig::http_block(void)
{
    return (_http_block);
}

std::vector<ConfigServerBlock>& ServerConfig::server_block_list(void)
{
    return (_server_block_list);
}

void ServerConfig::print_config(void) const
{
    _global_block.print_config();
    _http_block.print_config();
    for (size_t i = 0; i < _server_block_list.size(); ++i)
    {
        std::cout << "Server block [" << i << "]:" << std::endl;
        _server_block_list[i].print_config();
    }
}

void ServerConfig::parse(void)
{
    std::string line;

    while (!_file_stream.eof())
    {
        if (_current_block_level == GLOBAL)
            line = _global_block.parse(_file_stream);
        else if (_current_block_level == HTTP)
            line = _http_block.parse(_file_stream);
        else if (_current_block_level == SERVER)
            line = _server_block_list.back().parse(_file_stream);
        else if (_current_block_level == LOCATION)
            line = _server_block_list.back().location_block_list().back().parse(
                _file_stream);
        if (_set_block_level(line))
            continue;
    }
    if (_current_block_level != GLOBAL)
        throw std::invalid_argument(
            "Invalid block level: Make sure to close all blocks");
}

bool ServerConfig::_set_block_level(const std::string& line)
{
    if (line[0] == '{')
        _current_block_level = GLOBAL;
    else if (line.find("http {") != std::string::npos)
        _current_block_level = HTTP;
    else if (line.find("server {") != std::string::npos)
    {
        _current_block_level = SERVER;
        _server_block_list.push_back(ConfigServerBlock());
    }
    else if (line.find("location {") != std::string::npos)
    {
        _current_block_level = LOCATION;
        _server_block_list.back().location_block_list().push_back(
            ConfigLocationBlock());
    }
    else if (line.find("}") != std::string::npos)
    {
        int level = static_cast<int>(_current_block_level);
        if (level > 0)
            _current_block_level = static_cast<ConfigBlockLevel>(level - 1);
        else
            throw std::invalid_argument("Invalid block level");
    }
    else
        return (false);
    return (true);
}

} // namespace webconfig
