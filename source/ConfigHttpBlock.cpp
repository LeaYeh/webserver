#include "ConfigHttpBlock.hpp"
#include "defines.hpp"
#include "utils.hpp"

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

ConfigHttpBlock::~ConfigHttpBlock()
{
}

std::string ConfigHttpBlock::parse(std::ifstream& file_stream)
{
    std::string line;
    while (std::getline(file_stream, line))
    {
        if (!_is_valid_scentence(line))
            throw std::invalid_argument("HTTP scope has invalid line: " + line);
        if (_need_to_skip(line))
            continue;
        if (line.find("{") != std::string::npos)
            return (line);
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive))
            throw std::invalid_argument("HTTP scope has invalid directive: " +
                                        directive);
    }
    return "";
}

void ConfigHttpBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "client_max_body_size")
        _client_max_body_size = _parse_client_max_body_size(line);
}

unsigned int ConfigHttpBlock::_parse_client_max_body_size(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);

    return (utils::convert_to_size(value));
}

webshell::ContentType ConfigHttpBlock::_parse_default_type(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);

    return (get_content_type(value));
}

ErrorPage ConfigHttpBlock::_parse_error_page(const std::string& line)
{
    std::string directive = _get_directive_name(line);
    std::string value = extract_directive_value(line, directive);
    webshell::StatusCode status_code = convert_to_status_code(value);
    std::string path = extract_directive_value(line, value);

    return (ErrorPage(status_code, path));
}

} // namespace webconfig
