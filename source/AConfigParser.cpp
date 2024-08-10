#include "AConfigParser.hpp"
#include "utils.hpp"

namespace webconfig
{

AConfigParser::AConfigParser(ConfigBlockLevel block_level)
    : _block_level(block_level)
{
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

bool AConfigParser::_is_valid_scentence(const std::string& line) const
{
    if (_need_to_skip(line) || line.find('{') != std::string::npos)
        return (true);
    std::string trimmed_line = utils::trim(line);
    if (trimmed_line.find((';')) != trimmed_line.size() - 1)
        return (false);
    return (true);
}

bool AConfigParser::_is_valid_directive(const std::string& directive) const
{
    if (_valid_directives.find(directive) == _valid_directives.end())
        return (false);
    return (true);
}

} // namespace webconfig
