#include "UriAnalyzer.hpp"
#include "HttpException.hpp"

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
    _idx = 0;
    _state = URI_START;
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _scheme(other._scheme), _directory(other._directory), _query(other._query),
        _state(other._state), _idx(other._idx)
{
}

UriAnalyzer& UriAnalyzer::operator=(const UriAnalyzer& other)
{
    if (this != &other)
    {
        _scheme = other._scheme;
        _directory = other._directory;
        _query = other._query;
        _state = other._state;
        _idx = other._idx;
    }
    return (*this);
}

UriAnalyzer::~UriAnalyzer()
{
}

void UriAnalyzer::parse_uri(std::string& uri)
{
    //received path_empty. This is not a bad request but can't be processed.
    //TODO: if i throw not found here, i need to analyze everything else before
    //as bad_request should take precedence.
    if (uri.empty())
        throw utils::HttpException(webshell::NOT_FOUND,
                NOT_FOUND_MSG);
    std::string::const_iterator iter = uri.begin();
    while (iter < uri.end())
    {
        _feed(*iter);
        iter++;
    }
}

void UriAnalyzer::_feed(unsigned char c)
{
    (void)c;
    // switch (_state)
    // {

    // }
}

void UriAnalyzer::reset()
{
    _idx = _percent_idx = _percent_val = 0;
    _state = URI_START;
    //set strings to empty here
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
