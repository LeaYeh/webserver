#include "ConfigHttpBlock.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <cstddef>

namespace webconfig
{

ConfigHttpBlock::ConfigHttpBlock()
    : AConfigParser(HTTP), _client_max_body_size(0),
      _default_type(webshell::TEXT_PLAIN), _error_page_list(),
      _autoindex_page("./www/html/autoindex.html")
{
    _valid_directives.insert("client_max_body_size");
    _valid_directives.insert("default_type");
    _valid_directives.insert("error_page");
    _valid_directives.insert("autoindex_page");
}

ConfigHttpBlock::ConfigHttpBlock(const ConfigHttpBlock& other)
    : AConfigParser(other), _client_max_body_size(other._client_max_body_size),
      _default_type(other._default_type),
      _error_page_list(other._error_page_list), _autoindex_page(other._autoindex_page)
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
        _autoindex_page = other._autoindex_page;
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
        if (!_isValidSentence(line))
            throw std::invalid_argument("HTTP scope has invalid line: " + line);
        if (_needToSkip(line))
            continue;
        if (_isScopeSymbol(line))
            return (line);
        std::string directive = _getDirectiveName(line);
        if (!_isValidDirective(directive))
            throw std::invalid_argument("HTTP scope has invalid directive: " +
                                        directive);
        _parseConfigDirective(line);
    }
    return "";
}

void ConfigHttpBlock::printConfig(void) const
{
    weblog::Logger::log(weblog::DEBUG, "HTTP scope:");
    weblog::Logger::log(weblog::DEBUG,
                        "\tclient_max_body_size: " +
                            utils::toString(_client_max_body_size));
    weblog::Logger::log(weblog::DEBUG,
                        "\tdefault_type: " +
                            webshell::contentTypeToString(_default_type));
    weblog::Logger::log(weblog::DEBUG, "\terror_page:");
    for (size_t i = 0; i < _error_page_list.size(); ++i)
    {
        weblog::Logger::log(
            weblog::DEBUG,
            "\t\t" + utils::toString(_error_page_list[i].status_code) + " " +
                _error_page_list[i].path);
    }
    weblog::Logger::log(weblog::DEBUG, "\tautoindex_page: " + _autoindex_page);
}

unsigned int ConfigHttpBlock::clientMaxBodySize(void) const
{
    return (_client_max_body_size);
}

webshell::ContentType ConfigHttpBlock::defaultType(void) const
{
    return (_default_type);
}

std::vector<ErrorPage> ConfigHttpBlock::errorPages(void) const
{
    return (_error_page_list);
}

std::string ConfigHttpBlock::autoindexPage(void) const
{
    return (_autoindex_page);
}

void ConfigHttpBlock::_parseConfigDirective(const std::string& line)
{
    std::string directive = _getDirectiveName(line);

    if (directive == "client_max_body_size")
        _client_max_body_size = _parseClientMaxBodySize(line);
    else if (directive == "default_type")
        _default_type = _parseDefaultType(line);
    else if (directive == "error_page")
        _error_page_list.push_back(_parseErrorPage(line));
    else if (directive == "autoindex_page")
        _autoindex_page = _parseAutoindexPage(line);
    else
        throw std::invalid_argument("Invalid directive in HTTP block: " +
                                    directive);
}

unsigned int ConfigHttpBlock::_parseClientMaxBodySize(const std::string& line)
{
    std::string directive = _getDirectiveName(line);
    std::string value = extract_directive_value(line, directive);

    return (utils::convertToSize(value));
}

webshell::ContentType
ConfigHttpBlock::_parseDefaultType(const std::string& line)
{
    std::string directive = _getDirectiveName(line);
    std::string value = extract_directive_value(line, directive);

    return (string_to_content_type(value));
}

ErrorPage ConfigHttpBlock::_parseErrorPage(const std::string& line)
{
    std::string directive = _getDirectiveName(line);
    std::string value = extract_directive_value(line, directive);
    std::vector<std::string> args = utils::split(value, ' ');

    if (args.size() != 2)
        throw std::invalid_argument("error_page directive has invalid value: " +
                                    value);
    webshell::StatusCode status_code = string_to_status_code(args[0]);

    return (ErrorPage(status_code, args[1]));
}

std::string ConfigHttpBlock::_parseAutoindexPage(const std::string& line)
{
    std::string directive = _getDirectiveName(line);
    std::string value = extract_directive_value(line, directive);

    return (value);
}

} // namespace webconfig
