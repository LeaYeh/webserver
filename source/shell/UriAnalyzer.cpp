#include "UriAnalyzer.hpp"
#include "HttpException.hpp"

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
    _idx = 0;
    _sidx = 0;
    _state = URI_START;
    _path = "";
    _host = "";
    _port = "";
    _query = "";
    _fragment = "";
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _host(other._host), _port(other._port), _path(other._path),
        _query(other._query), _fragment(other._fragment), _state(other._state), _idx(other._idx)
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
        _fragment = other._fragment;
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
    // std::string::const_iterator iter = uri.begin();
    //Need to kill this iterator because erase() inside of the nest would ivalidate
    _uri = uri;
    while (_idx < _uri.size())
    {
        _feed(_uri[_idx]);
        _idx++;
    }
    if (_state < URI_REL_START) //TODO: is it host? or which state? always consider abempty
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
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
    else if (c == '%')
    {
        _path.push_back(_decode_percent());
        _state = URI_PATH;
        return;
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
        _state = URI_HOST_TRIAL;
    }
    else if (_is_pchar(c))
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else if (c == '%')
    {
        _path.push_back(_decode_percent());
        _state = URI_PATH;
        return;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
        BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_scheme(unsigned char c)
{
    _sidx++;
    if ((_sidx == 1 || _sidx == 2) && (c == 't' || c == 'T')) 
        return ;
    if (_sidx == 3 && (c == 'p' || c == 'P')) 
        return ;
    if (_sidx == 4 && c == ':') 
        return ;
    if (_sidx == 5 && c == '/') 
        return ;
    if (_sidx == 6 && c == '/') 
    {
        _sidx = 0;
        _state = URI_HOST_TRIAL;
        return;
    }
    throw utils::HttpException(webshell::BAD_REQUEST,
        BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_path(unsigned char c)
{
    //segment is 0 or more pchar, so we could in theory keep receiving just slashes,
    //except for the beginning which is reserved to signal authority, but i cover
    //that at URI_REL_START
    if (c == '/' || _is_pchar(c))
    {
        _path.push_back(c);
        return;
    }
    else if (c == '%')
    {
        _path.push_back(_decode_percent());
        return;
    }
    if (c == '?')
        _state = URI_QUERY;
    else if (c == '#')
        _state = URI_FRAGMENT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_query(unsigned char c)
{
    if (c == '/' || c == '?' || _is_pchar(c))
    {
        _query.push_back(c);
        return;
    }
    else if (c == '%')
    {
        _query.push_back(_decode_percent());
        return;
    }
    else if (c == '#')
        _state = URI_FRAGMENT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_fragment(unsigned char c)
{
    if (c == '/' || c == '?' || _is_pchar(c))
    {
        _fragment.push_back(c);
        return;
    }
    else if (c == '%')
    {
        _fragment.push_back(_decode_percent());
        return;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_path_trial(unsigned char c)
{
    if (_is_pchar(c))
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else if (c == '%')
    {
        _path.push_back(_decode_percent());
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void UriAnalyzer::_uri_port(unsigned char c)
{
    if (isdigit(c))
    {
        _port.push_back(c);
        return;
    }
    else if (c == '/')
    {
        _path.push_back(c);
        _state = URI_PATH_TRIAL;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

bool UriAnalyzer::_valid_hexdigit(unsigned char c)
{
    if (isdigit(c))
        return (true);
    if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
        return (true);
    if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')
        return (true);
    return (false);
}

unsigned char UriAnalyzer::_hexval(unsigned char c)
{
    if (isdigit(c))
        return (c - 48);
    if (c > 96)
        return (c - 'a' + 10);
    return (c - 'A' + 10);
}

unsigned char UriAnalyzer::_decode_percent()
{
    if (_idx > _uri.size() - 3)
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);

    unsigned char first = _uri[_idx + 1];
    unsigned char second = _uri[_idx + 2];
    if (!_valid_hexdigit(first) || !_valid_hexdigit(second))
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);

    _idx += 3;
    //TODO: is %FF a problem? would turn to 256
    return (_hexval(first) * 16 + _hexval(second));
}

void UriAnalyzer::_uri_host_trial(unsigned char c)
{
    if (c == '%')
        c = _decode_percent();
    if (isdigit(c))
    {
        _host.push_back(c);
        _state = URI_HOST_IPV4;
        return;
    }
    else if (_is_unreserved(c) || _is_sub_delim(c))
    {
        _host.push_back(c);
        _state = URI_HOST_REGNAME;
    }
    else
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
        case URI_HOST_IPV4:
            break;
        case URI_HOST_REGNAME:
            break;
        case URI_PORT:
            _uri_port(c);
            break;
        case URI_PATH_TRIAL:
            _uri_path_trial(c);
            break;
        case URI_PATH:
            _uri_path(c);
            break;
        case URI_QUERY: //str does not contain ?
            _uri_query(c);
            break;
        case URI_FRAGMENT: //str does not contain #
            _uri_fragment(c);
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
