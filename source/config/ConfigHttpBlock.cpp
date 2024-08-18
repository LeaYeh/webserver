#include "ConfigHttpBlock.hpp"
#include "defines.hpp"
#include "utils/utils.hpp"
#include "utils/Logger.hpp"
#include <cstddef>

namespace webconfig
{

ConfigHttpBlock::ConfigHttpBlock()
    : AConfigParser(HTTP), _client_max_body_size(0),
      _default_type(webshell::TEXT_PLAIN), _error_page_list()
{
    _valid_directives.insert("client_max_body_size");
    _valid_directives.insert("default_type");
    _valid_directives.insert("error_page");
}

ConfigHttpBlock::ConfigHttpBlock(const ConfigHttpBlock& other)
    : AConfigParser(other), _client_max_body_size(other._client_max_body_size),
      _default_type(other._default_type), _error_page_list(other._error_page_list)
{
}

ConfigHttpBlock& ConfigHttpBlock::operator=(const ConfigHttpBlock& other)
{
    if (this != &other)
    {
        AConfigParser::operator=(other);
        _client_max_body_size = other._client_max_body_size;
        _default_type = other._default_type;
        _error_page_list = other._error_page_list;
    }
    return (*this);
}

ConfigHttpBlock::~ConfigHttpBlock()
{
}

std::string ConfigHttpBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        if (!_is_valid_sentence(line))
            throw std::invalid_argument("HTTP scope has invalid line: " + line);
        if (_need_to_skip(line))
            continue;
        if (_is_scope_symbol(line))
            return (line);
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive))
            throw std::invalid_argument("HTTP scope has invalid directive: " +
                                        directive);
        _parse_config_directive(line);
    }
    return "";
}

void ConfigHttpBlock::print_config(void) const
{
    weblog::logger.log(weblog::DEBUG, "HTTP scope:");
    weblog::logger.log(weblog::DEBUG, "\tclient_max_body_size: " +
                                          utils::to_string(_client_max_body_size));
    weblog::logger.log(weblog::DEBUG, "\tdefault_type: " + utils::content_type_to_string(_default_type));
    weblog::logger.log(weblog::DEBUG, "\terror_page:");
    for (size_t i = 0; i < _error_page_list.size(); ++i)
    {
        weblog::logger.log(weblog::DEBUG, "\t\t" + utils::to_string(_error_page_list[i].status_code) + " " +
                                              _error_page_list[i].path);
    }
}

void ConfigHttpBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "client_max_body_size")
        _client_max_body_size = _parse_client_max_body_size(line);
    else if (directive == "default_type")
        _default_type = _parse_default_type(line);
    else if (directive == "error_page")
        _error_page_list.push_back(_parse_error_page(line));
    else
        throw std::invalid_argument("Invalid directive in HTTP block: " +
                                    directive);
}

unsigned int
ConfigHttpBlock::_parse_client_max_body_size(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);

    return (utils::convert_to_size(value));
}

webshell::ContentType
ConfigHttpBlock::_parse_default_type(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);

    return (string_to_content_type(value));
}

std::pair<webshell::StatusCode, std::string> ConfigHttpBlock::_parse_error_page(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);
    std::vector<std::string> args = utils::split(value, ' ');

    if (args.size() != 2)
        throw std::invalid_argument("error_page directive has invalid value: " +
                                    value);
    webshell::StatusCode status_code = string_to_status_code(args[0]);

    return (std::make_pair(status_code, args[1]));
}

} // namespace webconfig
