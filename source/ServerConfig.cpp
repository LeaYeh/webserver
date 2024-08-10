#include "ServerConfig.hpp"
#include "defines.hpp"

namespace webconfig
{

ServerConfig::ServerConfig(const std::string& filename)
    : _filename(filename), _current_block_level(GLOBAL)
{
    _file_stream.open(filename.c_str(), std::ifstream::in);
    if (!_file_stream.good() || !_file_stream.is_open())
        throw std::invalid_argument("Error opening file: " + filename);
}

ServerConfig::~ServerConfig()
{
    _file_stream.close();
}

void ServerConfig::parse(void)
{
    std::string line;

    while (std::getline(_file_stream, line) && _current_block_level != END)
    {
        std::cout << "line: |" << line << "|" << std::endl;

        if (_current_block_level == GLOBAL)
            line = _global_block.parse(_file_stream);
        else if (_current_block_level == HTTP)
            _http_block.parse(_file_stream);
        // else if (_current_block_level == SERVER)
        // {
        //     ConfigServerBlock server_block;
        //     server_block.parse(_file_stream);
        //     _server_block_list.push_back(server_block);
        // }
        if (line.empty() || _set_block_level(line))
            continue;
    }

    std::cout << "current block level: " << _current_block_level << std::endl;
    // if (_current_block_level != END)
    //     throw std::invalid_argument("Invalid block level: Make sure to close all blocks");
}

unsigned int ServerConfig::worker_processes(void) const
{
    return (_global_block.worker_processes);
}

unsigned int ServerConfig::worker_connections(void) const
{
    return (_global_block.worker_connections);
}

bool ServerConfig::_set_block_level(const std::string& line)
{
    if (line[0] == '{')
        _current_block_level = GLOBAL;
    else if (line.find("http {") != std::string::npos)
        _current_block_level = HTTP;
    else if (line.find("server {") != std::string::npos)
        _current_block_level = SERVER;
    else if (line.find("location") != std::string::npos &&
             line.find("{") != std::string::npos)
        _current_block_level = LOCATION;
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
