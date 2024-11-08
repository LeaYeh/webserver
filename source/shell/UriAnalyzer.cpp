#include "UriAnalyzer.hpp"

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _scheme(other._scheme), _directory(other._directory), _query(other._query)
{
}

UriAnalyzer& UriAnalyzer::operator=(const UriAnalyzer& other)
{
    if (this != &other)
    {
        _scheme = other._scheme;
        _directory = other._directory;
        _query = other._query;
    }
    return (*this);
}

UriAnalyzer::~UriAnalyzer()
{
}

void UriAnalyzer::parse_uri(const std::string& uri)
{
    (void)uri;
}

std::string UriAnalyzer::scheme() const
{
    return (_scheme);
}

std::string UriAnalyzer::directory() const
{
    return (_directory);
}

std::string UriAnalyzer::query() const
{
    return (_query);
}


bool UriAnalyzer::_is_unreserved(unsigned char c)
{
    if (isalpha(c))
        return (true);
    if (isdigit(c))
        return (true);
    if (c == '-' || c == '.' || c == '_' || c == '~')
        return (true);
    return (false);
}

bool UriAnalyzer::_is_sub_delim(unsigned char c)
{
    if (c == '!' || c == '$' || c == '&' || c == '\'')
        return (true);
    if (c == '(' || c == ')' || c == '*' || c == '+')
        return (true);
    if (c == ',' || c == ';' || c == '=')
        return (true);
    return (false);
}

bool UriAnalyzer::_is_gen_delim(unsigned char c)
{
    if (c == ':' || c == '/' || c == '?' || c == '#')
        return (true);
    if (c == '[' || c == ']' || c == '@')
        return (true);
    return (false);
}

bool UriAnalyzer::_is_pchar(unsigned char c, bool userinfo)
{
    if (_is_unreserved(c))
        return (true);
    //how to check for percent-encoded?? Need a state for that?
    if (_is_sub_delim(c))
        return (true);
    if (c == ':')
        return (true);
    if (c == '@' && !userinfo)
        return (true);
    return (false);
}

bool UriAnalyzer::_is_query_or_fragment_part(unsigned char c)
{
    if (_is_pchar(c, false))
        return (true);
    if (c == '/' || c == '?')
        return (true);
    return (false);
}

} // namespace webshell
