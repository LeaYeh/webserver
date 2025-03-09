#include "ConfigHttpBlock.hpp"
#include "Logger.hpp"
#include "configUtils.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <cstddef>
#include <string>

namespace webconfig
{

ConfigHttpBlock::ConfigHttpBlock() :
    AConfigParser(HTTP),
    _client_max_body_size(0),
    _default_type(webshell::TEXT_PLAIN),
    _autoindex_page("./www/html/autoindex.html")
{
    _valid_directives.insert("client_max_body_size");
    _valid_directives.insert("default_type");
    _valid_directives.insert("error_page");
    _valid_directives.insert("autoindex_page");
}

ConfigHttpBlock::ConfigHttpBlock(const ConfigHttpBlock& other) :
    AConfigParser(other),
    _client_max_body_size(other._client_max_body_size),
    _default_type(other._default_type),
    _autoindex_page(other._autoindex_page)
{
}

ConfigHttpBlock& ConfigHttpBlock::operator=(const ConfigHttpBlock& other)
{
    if (this != &other) {
        AConfigParser::operator=(other);
        _client_max_body_size = other._client_max_body_size;
        _default_type = other._default_type;
        _autoindex_page = other._autoindex_page;
    }
    return (*this);
}

ConfigHttpBlock::~ConfigHttpBlock() {}

std::string ConfigHttpBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line)) {
        if (!_is_valid_sentence(line)) {
            throw std::invalid_argument("HTTP scope has invalid line: " + line);
        }
        if (_need_to_skip(line)) {
            continue;
        }
        if (_is_scope_symbol(line)) {
            return (line);
        }
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive)) {
            throw std::invalid_argument("HTTP scope has invalid directive: "
                                        + directive);
        }
        _parse_config_directive(line);
    }
    return "";
}

void ConfigHttpBlock::print_config(void) const
{
    LOG(weblog::DEBUG, "HTTP scope:");
    LOG(weblog::DEBUG,
        "\tclient_max_body_size: " + utils::to_string(_client_max_body_size));
    LOG(weblog::DEBUG,
        "\tdefault_type: " + webshell::content_type_to_string(_default_type));
    LOG(weblog::DEBUG, "\tautoindex_page: " + _autoindex_page);
}

size_t ConfigHttpBlock::client_max_body_size(void) const
{
    return (_client_max_body_size);
}

webshell::ContentType ConfigHttpBlock::default_type(void) const
{
    return (_default_type);
}

std::string ConfigHttpBlock::autoindex_page(void) const
{
    return (_autoindex_page);
}

std::string ConfigHttpBlock::error_page(void) const
{
    return (_error_page);
}

void ConfigHttpBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "client_max_body_size") {
        _client_max_body_size = _parse_client_max_body_size(line);
    }
    else if (directive == "default_type") {
        _default_type = _parse_default_type(line);
    }
    else if (directive == "autoindex_page") {
        _autoindex_page = extract_directive_value(line, directive);
    }
    else if (directive == "error_page") {
        _error_page = extract_directive_value(line, directive);
    }
    else {
        throw std::invalid_argument("Invalid directive in HTTP block: "
                                    + directive);
    }
}

size_t ConfigHttpBlock::_parse_client_max_body_size(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);
    size_t size;

    try {
        size = utils::convert_to_size(value);
    }
    catch (const std::exception& e) {
        throw std::invalid_argument("client_max_body_size: invalid argument: "
                                    + value);
    }

    return (size);
}

webshell::ContentType
ConfigHttpBlock::_parse_default_type(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);

    return (string_to_content_type(value));
}

} // namespace webconfig
