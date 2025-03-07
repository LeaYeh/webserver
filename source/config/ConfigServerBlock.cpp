#include "ConfigServerBlock.hpp"
#include "Logger.hpp"
#include "configUtils.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include "IPAddress.hpp"
#include <string>

namespace webconfig
{

ConfigServerBlock::ConfigServerBlock() :
    AConfigParser(SERVER),
    _server_name("localhost"),
    _listen(std::make_pair(std::string("127.0.0.1"), std::string("80"))),
    _keep_alive_timeout(65),
    _is_location_block_sorted(false)
{
    _valid_directives.insert("server_name");
    _valid_directives.insert("listen");
    _valid_directives.insert("error_log");
    _valid_directives.insert("keep_alive_timeout");

    _error_log.push_back(std::make_pair("logs/error.log", weblog::ERROR));
}

ConfigServerBlock::ConfigServerBlock(const ConfigServerBlock& other) :
    AConfigParser(other),
    _server_name(other._server_name),
    _listen(other._listen),
    _error_log(other._error_log),
    _keep_alive_timeout(other._keep_alive_timeout),
    _is_location_block_sorted(other._is_location_block_sorted),
    _location_block_list(other._location_block_list)
{
}

ConfigServerBlock& ConfigServerBlock::operator=(const ConfigServerBlock& other)
{
    if (this != &other) {
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

ConfigServerBlock::~ConfigServerBlock() {}

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

size_t ConfigServerBlock::keep_alive_timeout(void) const
{
    return (_keep_alive_timeout);
}

std::vector<ConfigLocationBlock>& ConfigServerBlock::location_block_list(void)
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

    while (std::getline(file_stream, line)) {
        if (_need_to_skip(line)) {
            continue;
        }
        if (!_is_valid_sentence(line)) {
            throw std::invalid_argument("Server scope has invalid line: "
                                        + line);
        }
        if (_is_scope_symbol(line)) {
            return (line);
        }
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive)) {
            throw std::invalid_argument("Server scope has invalid directive: "
                                        + directive);
        }
        _parse_config_directive(line);
    }
    return "";
}

void ConfigServerBlock::print_config(void) const
{
    LOG(weblog::DEBUG, "Server block:");
    LOG(weblog::DEBUG, "\tServer name: " + _server_name);
    LOG(weblog::DEBUG, "\tListen: " + _listen.first + ":" + _listen.second);
    for (size_t i = 0; i < _error_log.size(); ++i) {
        LOG(weblog::DEBUG,
            "\tError log: " + _error_log[i].first + " "
                + level_to_string(_error_log[i].second));
    }
    LOG(weblog::DEBUG,
        "\tKeepalive timeout: " + utils::to_string(_keep_alive_timeout));
    for (size_t i = 0; i < _location_block_list.size(); ++i) {
        LOG(weblog::DEBUG, "Location block [" + utils::to_string(i) + "]:");
        _location_block_list[i].print_config();
    }
}

void ConfigServerBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "server_name") {
        _server_name = extract_directive_value(line, directive);
    }
    else if (directive == "listen") {
        _listen = _parse_listen(line, directive);
    }
    else if (directive == "error_log") {
        _error_log.push_back(_parse_error_log(line, directive));
    }
    else if (directive == "keep_alive_timeout") {
        _keep_alive_timeout = _parse_keep_alive_timeout(line, directive);
    }
}

std::pair<std::string, std::string>
ConfigServerBlock::_parse_listen(const std::string& line,
                                 const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(":");
    if (pos == std::string::npos) {
        throw std::invalid_argument("Listen directive has invalid value: "
                                    + value);
    }
    std::string ip = value.substr(0, pos);
    std::string port = value.substr(pos + 1);
    if (!utils::IPAddress::is_valid_ip(ip)) {
        throw std::invalid_argument("Listen directive has invalid IP: " + ip);
    }
    if (!utils::IPAddress::is_valid_port(port)) {
        throw std::invalid_argument("Listen directive has invalid port: "
                                    + port);
    }
    return (std::make_pair(value.substr(0, pos), value.substr(pos + 1)));
}

std::pair<std::string, weblog::LogLevel>
ConfigServerBlock::_parse_error_log(const std::string& line,
                                    const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    size_t pos = value.find(" ");
    if (pos == std::string::npos) {
        throw std::invalid_argument("Error log directive has invalid value: "
                                    + value);
    }
    return (std::make_pair(value.substr(0, pos),
                           string_to_level(value.substr(pos + 1))));
}

size_t
ConfigServerBlock::_parse_keep_alive_timeout(const std::string& line,
                                             const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    return (utils::stoi(value));
}

} // namespace webconfig
