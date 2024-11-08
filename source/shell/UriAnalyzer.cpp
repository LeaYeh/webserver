#include "UriAnalyzer.hpp"
#include "HttpException.hpp"

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
    _idx = 0;
    _state = URI_START;
    _path = "";
    _host = "";
    _port = "";
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _host(other._host), _port(other._port), _path(other._path),
        _query(other._query), _state(other._state), _idx(other._idx)
{
}

UriAnalyzer& UriAnalyzer::operator=(const UriAnalyzer& other)
{
    if (this != &other)
    {
        _host = other._host;
        _port = other._port;
        _path = other._path;
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
    _state = END_URI_PARSER;
}

void UriAnalyzer::_uri_start(unsigned char c)
{
    if (c == 'h' || c == 'H')
        _state = URI_SCHEME;
    else if (c == '/') //path-empty already covered
    {
        _path.push_back(c);
        _state = URI_REL_START;
    }
    else if (_is_pchar(c))
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
        BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_rel_start(unsigned char c)
{
    if (c == '/')
    {
        _path.erase(_path.size() - 1);
        _state = URI_HOST;
    }
    else if (_is_pchar(c))
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
        BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_scheme(unsigned char c)
{
    _idx++;
    if ((_idx == 1 || _idx == 2) && (c == 't' || c == 'T')) 
        return ;
    if (_idx == 3 && (c == 'p' || c == 'P')) 
        return ;
    if (_idx == 4 && c == ':') 
        return ;
    if (_idx == 5 && c == '/') 
        return ;
    if (_idx == 6 && c == '/') 
    {
        _idx = 0;
        _state = URI_HOST;
        return;
    }
    throw utils::HttpException(webshell::BAD_REQUEST,
        BAD_REQUEST_MSG);
}

void UriAnalyzer::_feed(unsigned char c)
{
    //TODO: handle percent-encoding first!!
    switch (_state)
    {
        case URI_START:
            _uri_start(c);
            break;
        case URI_REL_START:
            _uri_rel_start(c);
            break;
        case URI_SCHEME:
            _uri_scheme(c);
            break;
        case URI_HOST:
            break;
        case URI_PORT:
            break;
        case URI_PATH:
            break;
        case URI_QUERY:
            break;
        case URI_FRAGMENT:
            break; 
        default:
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                "Feed at URIAnalyzer failed");
    }
}

void UriAnalyzer::reset()
{
    _idx = _percent_idx = _percent_val = 0;
    _state = URI_START;
    //set strings to empty here
}

// std::string UriAnalyzer::scheme() const
// {
//     return (_scheme);
// }

// std::string UriAnalyzer::directory() const
// {
//     return (_directory);
// }

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

bool UriAnalyzer::_is_pchar(unsigned char c/*, bool userinfo*/)
{
    if (_is_unreserved(c))
        return (true);
    //how to check for percent-encoded?? Need a state for that?
    if (_is_sub_delim(c))
        return (true);
    if (c == ':')
        return (true);
    // if (c == '@' && !userinfo)
    //     return (true);
    return (false);
}

bool UriAnalyzer::_is_query_or_fragment_part(unsigned char c)
{
    if (_is_pchar(c))
        return (true);
    if (c == '/' || c == '?')
        return (true);
    return (false);
}

} // namespace webshell
