#include "ConfigServerBlock.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"
#include <string>

namespace webconfig
{

ConfigServerBlock::ConfigServerBlock()
    : AConfigParser(SERVER), _server_name("localhost"),
      _listen(std::make_pair(std::string("127.0.0.1"), std::string("80"))),
      _keep_alive_timeout(65)
{
    _valid_directives.insert("server_name");
    _valid_directives.insert("listen");
    _valid_directives.insert("error_log");
    _valid_directives.insert("keep_alive_timeout");

    _error_log.push_back(std::make_pair("logs/error.log", weblog::ERROR));
}

ConfigServerBlock::ConfigServerBlock(const ConfigServerBlock& other)
    : AConfigParser(other), _server_name(other._server_name),
      _listen(other._listen), _error_log(other._error_log),
      _keep_alive_timeout(other._keep_alive_timeout),
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
        _keep_alive_timeout = other._keep_alive_timeout;
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

std::pair<std::string, std::string> ConfigServerBlock::listen(void) const
{
    return (_listen);
}

std::vector<std::pair<std::string, weblog::LogLevel> >
ConfigServerBlock::error_log(void) const
{
    return (_error_log);
}

unsigned int ConfigServerBlock::keep_alive_timeout(void) const
{
    return (_keep_alive_timeout);
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
    weblog::logger.log(weblog::DEBUG, "Server block:");
    weblog::logger.log(weblog::DEBUG, "\tServer name: " + _server_name);
    weblog::logger.log(weblog::DEBUG,
                       "\tListen: " + _listen.first + ":" + _listen.second);
    for (size_t i = 0; i < _error_log.size(); ++i)
    {
        weblog::logger.log(weblog::DEBUG,
                           "\tError log: " + _error_log[i].first + " " +
                               level_to_string(_error_log[i].second));
    }
    weblog::logger.log(weblog::DEBUG,
                       "\tKeepalive timeout: " +
                           utils::to_string(_keep_alive_timeout));
    for (size_t i = 0; i < _location_block_list.size(); ++i)
    {
        weblog::logger.log(weblog::DEBUG,
                           "Location block [" + utils::to_string(i) + "]:");
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
    else if (directive == "keep_alive_timeout")
        _keep_alive_timeout = _parse_keep_alive_timeout(line, directive);
}

std::pair<std::string, std::string>
ConfigServerBlock::_parse_listen(const std::string& line,
                                 const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(":");
    if (pos == std::string::npos)
        return std::make_pair(value, std::string("80"));
    return (std::make_pair(value.substr(0, pos), value.substr(pos + 1)));
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
ConfigServerBlock::_parse_keep_alive_timeout(const std::string& line,
                                             const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    return (utils::stoi(value));
}

} // namespace webconfig
