#include "ConfigServerBlock.hpp"
#include "defines.hpp"
#include "utils/utils.hpp"

namespace webconfig
{

ConfigServerBlock::ConfigServerBlock()
    : AConfigParser(SERVER), _server_name("localhost"),
      _listen(std::make_pair("127.0.0.1", 80)), _keepalive_timeout(65)
{
    _valid_directives.insert("server_name");
    _valid_directives.insert("listen");
    _valid_directives.insert("error_log");
    _valid_directives.insert("keepalive_timeout");

    _error_log.push_back(std::make_pair("logs/error.log", weblog::ERROR));
}

ConfigServerBlock::ConfigServerBlock(const ConfigServerBlock& other)
    : AConfigParser(other), _server_name(other._server_name),
      _listen(other._listen), _error_log(other._error_log),
      _keepalive_timeout(other._keepalive_timeout),
      _location_block_list(other._location_block_list)
{
}

ConfigServerBlock& ConfigServerBlock::operator=(const ConfigServerBlock& other)
{
    if (this != &other)
    {
        AConfigParser::operator=(other);
        _server_name = other._server_name;
        _listen = other._listen;
        _error_log = other._error_log;
        _keepalive_timeout = other._keepalive_timeout;
        _location_block_list = other._location_block_list;
    }
    return (*this);
}

ConfigServerBlock::~ConfigServerBlock()
{
}

std::string ConfigServerBlock::server_name(void) const
{
    return (_server_name);
}

std::pair<std::string, unsigned int> ConfigServerBlock::listen(void) const
{
    return (_listen);
}

std::vector<std::pair<std::string, weblog::LogLevel> >
ConfigServerBlock::error_log(void) const
{
    return (_error_log);
}

unsigned int ConfigServerBlock::keepalive_timeout(void) const
{
    return (_keepalive_timeout);
}

std::vector<ConfigLocationBlock>& ConfigServerBlock::location_block_list(void)
{
    return (_location_block_list);
}

std::string ConfigServerBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        // std::cout << "SERVER line: " << line << std::endl;
        if (_need_to_skip(line))
            continue;
        if (!_is_valid_sentence(line))
            throw std::invalid_argument("Server scope has invalid line: " +
                                        line);
        if (_is_scope_symbol(line))
            return (line);
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive))
            throw std::invalid_argument("Server scope has invalid directive: " +
                                        directive);
        _parse_config_directive(line);
    }
    return "";
}

void ConfigServerBlock::print_config(void) const
{
    std::cout << "\tServer name: " << _server_name << std::endl;
    std::cout << "\tListen: " << _listen.first << ":" << _listen.second
              << std::endl;
    for (size_t i = 0; i < _error_log.size(); ++i)
    {
        std::cout << "\tError log: " << _error_log[i].first << " "
                  << level_to_string(_error_log[i].second) << std::endl;
    }
    std::cout << "\tKeepalive timeout: " << _keepalive_timeout << std::endl;
    for (size_t i = 0; i < _location_block_list.size(); ++i)
    {
        std::cout << "\tLocation block [" << i << "]:" << std::endl;
        _location_block_list[i].print_config();
    }
}

void ConfigServerBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "server_name")
        _server_name = extract_directive_value(line, directive);
    else if (directive == "listen")
        _listen = _parse_listen(line, directive);
    else if (directive == "error_log")
        _error_log.push_back(_parse_error_log(line, directive));
    else if (directive == "keepalive_timeout")
        _keepalive_timeout = _parse_keepalive_timeout(line, directive);
}

std::pair<std::string, unsigned int>
ConfigServerBlock::_parse_listen(const std::string& line,
                                 const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(":");
    if (pos == std::string::npos)
        return std::make_pair(value, 80);
    return (std::make_pair(value.substr(0, pos),
                           utils::stoi(value.substr(pos + 1))));
}

std::pair<std::string, weblog::LogLevel>
ConfigServerBlock::_parse_error_log(const std::string& line,
                                    const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(" ");
    if (pos == std::string::npos)
        throw std::invalid_argument("Error log directive has invalid value: " +
                                    value);
    return (std::make_pair(value.substr(0, pos),
                           string_to_level(value.substr(pos + 1))));
}

unsigned int
ConfigServerBlock::_parse_keepalive_timeout(const std::string& line,
                                            const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    return (utils::stoi(value));
}

} // namespace webconfig
