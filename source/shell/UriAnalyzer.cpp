/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UriAnalyzer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 18:21:05 by mhuszar           #+#    #+#             */
/*   Updated: 2024/12/20 21:37:40 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UriAnalyzer.hpp"
#include "HttpException.hpp"
#include <cstddef>

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
    _idx = 0;
    _sidx = 0;
    _ipv_digit = false;
    _ipv_dot = 0;
    _state = URI_START;
    _uri = "";
    _path = "";
    _host = "";
    _port = "";
    _query = "";
    _fragment = "";
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _uri(other._uri), _host(other._host), _port(other._port), _path(other._path),
        _query(other._query), _fragment(other._fragment), _state(other._state), _idx(other._idx),
            _sidx(other._sidx), _ipv_digit(other._ipv_digit), _ipv_dot(other._ipv_dot)
{
}

UriAnalyzer& UriAnalyzer::operator=(const UriAnalyzer& other)
{
    if (this != &other)
    {
        _uri = other._uri;
        _host = other._host;
        _port = other._port;
        _path = other._path;
        _query = other._query;
        _fragment = other._fragment;
        _state = other._state;
        _idx = other._idx;
        _sidx = other._sidx;
        _ipv_digit = other._ipv_digit;
        _ipv_dot = other._ipv_dot;
    }
    return (*this);
}

UriAnalyzer::~UriAnalyzer()
{
}

void UriAnalyzer::reset()
{
    _idx = 0;
    _sidx = 0;
    _ipv_digit = false;
    _ipv_dot = 0;
    _state = URI_START;
    _uri = "";
    _path = "";
    _host = "";
    _port = "";
    _query = "";
    _fragment = "";
}

void UriAnalyzer::_remove_last_segment(std::string& str) const
{
    size_t pos = str.find_last_of('/');
    if (pos != std::string::npos)
        str.erase(pos);
}

void UriAnalyzer::_move_first_segment(std::string& from, std::string& to) const
{
    size_t pos = from[0] == '/';
    pos = from.find('/', pos);
    if (pos != std::string::npos)
    {
        to += from.substr(0, pos);
        from = from.substr(pos);
    }
    else
    {
        to += from;
        from.clear();
    }
}

std::string UriAnalyzer::_remove_dot_segments() const
{
    std::string input_buffer = _path;
    std::string output_buffer;

    while (!input_buffer.empty())
    {
        if (input_buffer.substr(0, 3) == "../")
            input_buffer = input_buffer.substr(3);
        else if (input_buffer.substr(0, 2) == "./")
            input_buffer = input_buffer.substr(2);
        else if (input_buffer.substr(0, 3) == "/./")
            input_buffer = input_buffer.substr(2);
        else if (input_buffer == "/.")
            input_buffer = "/";
        else if (input_buffer.substr(0, 4) == "/../")
        {
            input_buffer = input_buffer.substr(3);
            _remove_last_segment(output_buffer);
        }
        else if (input_buffer == "/..")
        {
            input_buffer = "/";
            _remove_last_segment(output_buffer);
        }
        else if (input_buffer == ".." || input_buffer == ".")
            input_buffer.clear();
        else
            _move_first_segment(input_buffer, output_buffer);
    }
    return (output_buffer);
}

void UriAnalyzer::_decode(std::string& str)
{
    std::string buf;
    _idx = 0;
    while (_idx < str.size())
    {
        if (str[_idx] == '%')
            buf.push_back(_decode_percent(str));
        else
        {
            buf.push_back(str[_idx]);
            _idx++;
        }
    }
    str = buf;
}

void UriAnalyzer::_percent_decode_all()
{
    _decode(_uri);
    _decode(_host);
    _decode(_port);
    _decode(_path);
    _decode(_query);
}

Uri UriAnalyzer::take_uri() const
{
    Uri ret;
    ret.raw = _uri;
    // ret.scheme = "http://";
    ret.authority = _host + ":" + _port;
    ret.host = _host;
    ret.port = _port;
    ret.path = _path;
    ret.query = _query;
    ret.fragment = _fragment;
    return (ret);
}

void UriAnalyzer::parse_uri(std::string& uri)
{
    //received path_empty. This is not a bad request but can't be processed.
    if (uri.empty())
        throw utils::HttpException(webshell::NOT_FOUND,
                NOT_FOUND_MSG);
    _uri = uri;
    while (_idx < _uri.size())
    {
        _feed(_uri[_idx]);
        _idx++;
    }
    if (_state < URI_REL_START) //TODO: is it host? or which state? always consider abempty
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed: end too early");
    _percent_decode_all();
    //TODO: talk about this with team!! Technically i should switch these two lines.
    //But that means they can try to trick us and get out of root.
    _path = _remove_dot_segments();
    _state = END_URI_PARSER;
}

void UriAnalyzer::_feed(unsigned char c)
{
    if (c == '%')
        c = _decode_num_and_alpha();
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
        case URI_HOST_TRIAL:
            _uri_host_trial(c);
            break;
        case URI_HOST_IPV4:
            _uri_host_ipv4(c);
            break;
        case URI_HOST_REGNAME:
            _uri_host_regname(c);
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
        {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                "Feed at URIAnalyzer failed");
        }
            
    }
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
    else if (_is_pchar(c) || c == '%')
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
        "URIAnalyzer failed at uri start");
}

void UriAnalyzer::_uri_rel_start(unsigned char c)
{
    if (c == '/')
    {
        _path.erase(_path.size() - 1);
        _state = URI_HOST_TRIAL;
    }
    else if (_is_pchar(c) || c == '%')
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
        "URIAnalyzer failed at uri rel start");
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
        "URIAnalyzer failed at uri scheme");
}

void UriAnalyzer::_uri_host_trial(unsigned char c)
{
    _host.push_back(c);
    if (isdigit(c))
        _state = URI_HOST_IPV4;
    else if (_is_unreserved(c) || _is_sub_delim(c) || c == '%')
        _state = URI_HOST_REGNAME;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at host trial");
}

void UriAnalyzer::_uri_host_ipv4(unsigned char c)
{
    if (isdigit(c))
    {
        _ipv_digit = true;
        _host.push_back(c);
    }
    else if (c == '.')
    {
        if (!_ipv_digit)
            goto except;
        else
            _ipv_digit = false;
        _ipv_dot++;
        _host.push_back(c);
    }
    else if (c == ':')
        _state = URI_PORT;
    else if (c == '/')
    {
        _path.push_back(c);
        _state = URI_PATH_TRIAL;
    }
    else
        goto except;
    if (_ipv_dot > 3)
        goto except;
    
    return;

    except:

        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at ipv4 host");
}

void UriAnalyzer::_uri_host_regname(unsigned char c)
{
    if (_is_unreserved(c) || _is_sub_delim(c) || c == '%')
        _host.push_back(c);
    else if (c == '/')
    {
        _path.push_back(c);
        _state = URI_PATH_TRIAL;
    }
    else if (c == ':')
        _state = URI_PORT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at regname host");
}

void UriAnalyzer::_uri_port(unsigned char c)
{
    if (isdigit(c))
        _port.push_back(c);
    else if (c == '/')
    {
        _path.push_back(c);
        _state = URI_PATH_TRIAL;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at uri port");
}

void UriAnalyzer::_uri_path_trial(unsigned char c)
{
    if (_is_pchar(c) || c == '%')
    {
        _path.push_back(c);
        _state = URI_PATH;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at uri path tiral");
}

void UriAnalyzer::_uri_path(unsigned char c)
{
    //segment is 0 or more pchar, so we could in theory keep receiving just slashes,
    //except for the beginning which is reserved to signal authority start, but i cover
    //that at URI_REL_START
    if (c == '/' || _is_pchar(c) || c == '%')
        _path.push_back(c);
    else if (c == '?')
        _state = URI_QUERY;
    else if (c == '#')
        _state = URI_FRAGMENT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at uri path");
}

void UriAnalyzer::_uri_query(unsigned char c)
{
    if (_is_query_or_fragment_part(c) || c == '%')
        _query.push_back(c);
    else if (c == '#')
        _state = URI_FRAGMENT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at uri query");
}

void UriAnalyzer::_uri_fragment(unsigned char c)
{
    if (_is_query_or_fragment_part(c) || c == '%')
        _fragment.push_back(c);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed at uri fragment");
}

unsigned char UriAnalyzer::_decode_percent(std::string& str)
{
    if (_idx > str.size() - 3)
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed: percent code cut short");

    unsigned char first = str[_idx + 1];
    unsigned char second = str[_idx + 2];
    _idx += 3;
    return (_hexval(first) * 16 + _hexval(second));
}

unsigned char UriAnalyzer::_decode_num_and_alpha()
{
    if (_idx > _uri.size() - 3)
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed: percent code cut short");

    unsigned char first = _uri[_idx + 1];
    unsigned char second = _uri[_idx + 2];
    if (!_valid_hexdigit(first) || !_valid_hexdigit(second))
        throw utils::HttpException(webshell::BAD_REQUEST,
            "URIAnalyzer failed: not a valid hexdigit at encoding");

    unsigned char value = _hexval(first) * 16 + _hexval(second);
    if (_is_unreserved(value))
    {
        _idx += 3;
        return (value);
    }
    return ('%');
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

//TODO: move the following 2 to utils. Duplicate version exists in HeaderFieldValidator.
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

bool UriAnalyzer::_is_pchar(unsigned char c)
{
    if (_is_unreserved(c))
        return (true);
    //how to check for percent-encoded?? Need a state for that?
    if (_is_sub_delim(c))
        return (true);
    if (c == ':')
        return (true);
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
