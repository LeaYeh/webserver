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
      _keep_alive_timeout(65), _is_location_block_sorted(false)
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
      _is_location_block_sorted(other._is_location_block_sorted),
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
        _is_location_block_sorted = other._is_location_block_sorted;
        _location_block_list = other._location_block_list;
    }
    return (*this);
}

ConfigServerBlock::~ConfigServerBlock()
{
}

std::string ConfigServerBlock::serverName(void) const
{
    return (_server_name);
}

std::pair<std::string, std::string> ConfigServerBlock::listen(void) const
{
    return (_listen);
}

std::vector<std::pair<std::string, weblog::LogLevel> >
ConfigServerBlock::errorLog(void) const
{
    return (_error_log);
}

unsigned int ConfigServerBlock::keepAliveTimeout(void) const
{
    return (_keep_alive_timeout);
}

std::vector<ConfigLocationBlock>& ConfigServerBlock::locationBlockList(void)
{
    // if (!_is_location_block_sorted)
    // {
    //     std::sort(_location_block_list.begin(), _location_block_list.end());
    //     _is_location_block_sorted = true;
    // }
    return (_location_block_list);
}

std::string ConfigServerBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        // std::cout << "SERVER line: " << line << std::endl;
        if (_needToSkip(line))
            continue;
        if (!_isValidSentence(line))
            throw std::invalid_argument("Server scope has invalid line: " +
                                        line);
        if (_isScopeSymbol(line))
            return (line);
        std::string directive = _getDirectiveName(line);
        if (!_isValidDirective(directive))
            throw std::invalid_argument("Server scope has invalid directive: " +
                                        directive);
        _parseConfigDirective(line);
    }
    return "";
}

void ConfigServerBlock::printConfig(void) const
{
    weblog::Logger::log(weblog::DEBUG, "Server block:");
    weblog::Logger::log(weblog::DEBUG, "\tServer name: " + _server_name);
    weblog::Logger::log(weblog::DEBUG,
                        "\tListen: " + _listen.first + ":" + _listen.second);
    for (size_t i = 0; i < _error_log.size(); ++i)
    {
        weblog::Logger::log(weblog::DEBUG,
                            "\tError log: " + _error_log[i].first + " " +
                                level_to_string(_error_log[i].second));
    }
    weblog::Logger::log(weblog::DEBUG,
                        "\tKeepalive timeout: " +
                            utils::toString(_keep_alive_timeout));
    for (size_t i = 0; i < _location_block_list.size(); ++i)
    {
        weblog::Logger::log(weblog::DEBUG,
                            "Location block [" + utils::toString(i) + "]:");
        _location_block_list[i].printConfig();
    }
}

void ConfigServerBlock::_parseConfigDirective(const std::string& line)
{
    std::string directive = _getDirectiveName(line);

    if (directive == "server_name")
        _server_name = extract_directive_value(line, directive);
    else if (directive == "listen")
        _listen = _parseListen(line, directive);
    else if (directive == "error_log")
        _error_log.push_back(_parseErrorLog(line, directive));
    else if (directive == "keep_alive_timeout")
        _keep_alive_timeout = _parseKeepAliveTimeout(line, directive);
}

std::pair<std::string, std::string>
ConfigServerBlock::_parseListen(const std::string& line,
                                const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(":");
    if (pos == std::string::npos)
        return std::make_pair(value, std::string("80"));
    return (std::make_pair(value.substr(0, pos), value.substr(pos + 1)));
}

std::pair<std::string, weblog::LogLevel>
ConfigServerBlock::_parseErrorLog(const std::string& line,
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
ConfigServerBlock::_parseKeepAliveTimeout(const std::string& line,
                                          const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    return (utils::stoi(value));
}

} // namespace webconfig
