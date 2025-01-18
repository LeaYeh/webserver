#include "Config.hpp"
#include "ConfigLocationBlock.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <algorithm>
#include <vector>

namespace webconfig
{

Config::~Config()
{
}

Config* Config::create_instance()
{
    return (new Config());
}

Config* Config::create_instance(const std::string& filename)
{
    return (new Config(filename));
}

Config::Config() : _current_block_level(GLOBAL) {}

Config::Config(const std::string& filename) : _current_block_level(GLOBAL)
{
    _filename = filename;
    _file_stream.open(_filename.c_str());
    if (!_file_stream.is_open() || !_file_stream.good()) {
        throw std::runtime_error("Failed to open file: " + _filename);
    }
    _parse();
}

std::string Config::filename(void) const
{
    return (_filename);
}

const ConfigGlobalBlock& Config::global_block(void) const
{
    return (_global_block);
}

const ConfigHttpBlock& Config::http_block(void) const
{
    return (_http_block);
}

std::vector<ConfigServerBlock>& Config::server_block_list(void)
{
    return (_server_block_list);
}

const std::vector<ConfigServerBlock>& Config::server_block_list(void) const
{
    return (_server_block_list);
}

void Config::print_config(void) const
{
    _global_block.print_config();
    _http_block.print_config();
    for (size_t i = 0; i < _server_block_list.size(); ++i) {
        weblog::Logger::log(weblog::DEBUG,
                            "Server block [" + utils::to_string(i) + "]:");
        _server_block_list[i].print_config();
    }
}

void Config::_parse(void)
{
    std::string line;

    while (!_file_stream.eof()) {
        if (_current_block_level == GLOBAL) {
            line = _global_block.parse(_file_stream);
        }
        else if (_current_block_level == HTTP) {
            line = _http_block.parse(_file_stream);
        }
        else if (_current_block_level == SERVER) {
            line = _server_block_list.back().parse(_file_stream);
        }
        else if (_current_block_level == LOCATION) {
            line = _server_block_list.back().location_block_list().back().parse(
                _file_stream);
        }
        if (_set_block_level(line)) {
            continue;
        }
    }
    if (_current_block_level != GLOBAL) {
        throw std::invalid_argument(
            "Invalid block level: Make sure to close all blocks");
    }
    for (size_t i = 0; i < _server_block_list.size(); ++i) {
        std::vector<ConfigLocationBlock>& location_block_list =
            _server_block_list[i].location_block_list();
        std::sort(location_block_list.begin(), location_block_list.end());
    }
}

bool Config::_set_block_level(const std::string& line)
{
    if (line[0] == '{') {
        _current_block_level = GLOBAL;
    }
    else if (line.find("http {") != std::string::npos) {
        _current_block_level = HTTP;
    }
    else if (line.find("server {") != std::string::npos) {
        _current_block_level = SERVER;
        _server_block_list.push_back(ConfigServerBlock());
    }
    else if (line.find("location {") != std::string::npos) {
        _current_block_level = LOCATION;
        _server_block_list.back().location_block_list().push_back(
            ConfigLocationBlock());
    }
    else if (line.find("}") != std::string::npos) {
        int level = static_cast<int>(_current_block_level);
        if (level > 0) {
            _current_block_level = static_cast<ConfigBlockLevel>(level - 1);
        }
        else {
            throw std::invalid_argument("Invalid block level");
        }
    }
    else {
        return (false);
    }
    return (true);
}

} // namespace webconfig
