#include "AConfigParser.hpp"
#include "defines.hpp"
#include "utils/utils.hpp"

namespace webconfig
{

AConfigParser::AConfigParser() : _block_level(GLOBAL)
{
}

AConfigParser::AConfigParser(ConfigBlockLevel block_level)
    : _block_level(block_level)
{
}

AConfigParser::AConfigParser(const AConfigParser& other)
    : _block_level(other._block_level), _valid_directives(other._valid_directives)
{
}

AConfigParser& AConfigParser::operator=(const AConfigParser& other)
{
    if (this != &other)
    {
        _block_level = other._block_level;
        _valid_directives = other._valid_directives;
    }
    return (*this);
}

AConfigParser::~AConfigParser()
{
}

bool AConfigParser::_need_to_skip(const std::string& line) const
{
    std::string trimmed_line = utils::trim(line);

    return (trimmed_line.empty() || trimmed_line[0] == '#' ||
            trimmed_line[0] == ';');
}

std::string AConfigParser::_get_directive_name(const std::string& line) const
{
    std::string trimmed_line = utils::trim(line);
    std::string directive_name =
        trimmed_line.substr(0, trimmed_line.find_first_of(' '));

    return (directive_name);
}

bool AConfigParser::_is_valid_sentence(const std::string& line) const
{
    if (_need_to_skip(line) || line.find('{') != std::string::npos)
        return (true);
    std::string trimmed_line = utils::trim(line);

    if (trimmed_line[trimmed_line.size() - 1] != ';' &&
        trimmed_line[trimmed_line.size() - 1] != '}')
        return (false);
    return (true);
}

bool AConfigParser::_is_valid_directive(const std::string& directive) const
{
    if (_valid_directives.find(directive) == _valid_directives.end())
        return (false);
    return (true);
}

bool AConfigParser::_is_scope_symbol(const std::string& line) const
{
    std::string trimmed_line = utils::trim(line);

    return (trimmed_line[trimmed_line.size() - 1] == '{' ||
            trimmed_line[trimmed_line.size() - 1] == '}');
}

} // namespace webconfig
