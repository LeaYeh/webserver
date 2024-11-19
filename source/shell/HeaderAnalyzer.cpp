/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderAnalyzer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 18:50:44 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/19 12:48:07 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderAnalyzer.hpp"
#include "HttpException.hpp"
#include "utils.hpp"

namespace webshell
{

//TODO: reset() function!!

HeaderAnalyzer::HeaderAnalyzer()/* : _connection_type(KEEP_ALIVE)*/
{
    _state = START_FIELD_NAME;
    _key = "";
    _val = "";
}

HeaderAnalyzer::HeaderAnalyzer(const HeaderAnalyzer& other)
    : _state(other._state), _map(other._map), _key(other._key), _val(other._val)
{
}

HeaderAnalyzer& HeaderAnalyzer::operator=(const HeaderAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _map = other._map;
        _key = other._key;
        _val = other._val;
    }
    return (*this);
}

HeaderAnalyzer::~HeaderAnalyzer()
{
}

void HeaderAnalyzer::reset()
{
    _state = START_FIELD_NAME;
    _key.clear();
    _val.clear();
    _map.clear();
}

void HeaderAnalyzer::feed(unsigned char c)
{
    switch (_state)
    {
        case START_FIELD_NAME:
            _start_header(c);
            break;
        case FIELD_NAME:
            _field_name(c);
            break;
        case LEADING_WS:
            _leading_ws(c);
            break;
        case FIELD_VALUE:
            _field_val(c);
            break;
        case MIDDLE_OR_END_WS:
            _middle_or_end_ws(c);
            break;
        case FIELD_END_CRLF:
            _field_end_crlf(c);
            break;
        case HEADER_END_CRLF:
            _header_end_crlf(c);
            break;
        default:
            throw std::runtime_error("Header analyzing went wrong");
    }
}

bool HeaderAnalyzer::_is_ows(unsigned char c)
{
    if (c == ' ' || c == '\t')
        return (true);
    return (false);
}

unsigned char HeaderAnalyzer::_lowcase(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return (c += 32);
    return (c);
}

bool HeaderAnalyzer::_is_vchar(unsigned char c)
{
    if (c > 32 && c < 127) //32 is technically printable but its part of ows
        return (true);
    return (false);
}

void HeaderAnalyzer::_start_header(unsigned char c)
{
    if (utils::is_tchar(c))
    {
        _key.push_back(_lowcase(c));
        _state = FIELD_NAME;
    }
    else if (c == '\r')
        _state = HEADER_END_CRLF;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void HeaderAnalyzer::_field_name(unsigned char c)
{
    if (utils::is_tchar(c))
        _key.push_back(_lowcase(c));
    else if (c == ':')
        _state = LEADING_WS;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void HeaderAnalyzer::_leading_ws(unsigned char c)
{
    if (_is_ows(c))
        return ;
    else if (_is_vchar(c))
    {
        _val.push_back(_lowcase(c));
        _state = FIELD_VALUE;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
    
}

void HeaderAnalyzer::_field_val(unsigned char c)
{
    if (_is_vchar(c))
        _val.push_back(_lowcase(c));
    else if (_is_ows(c))
        _state = MIDDLE_OR_END_WS;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

void HeaderAnalyzer::_middle_or_end_ws(unsigned char c)
{
    if (_is_vchar(c))
    {
        _val.push_back(' '); //TODO: should i separate like this or not at all?
        _val.push_back(_lowcase(c));
        _state = FIELD_VALUE;
    }
    else if (_is_ows(c))
        return ;
    else if (c == '\r')
        _state = FIELD_END_CRLF;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

// void HeaderAnalyzer::_trailing_ws(unsigned char c)
// {
//     (void)c;
// }

void HeaderAnalyzer::_field_end_crlf(unsigned char c)
{
    if (c == '\n')
    {
        _map[_key] = _val;
        _key.clear();
        _val.clear();
        // _state = LIMBO;
        _state = START_FIELD_NAME;
    }
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

// void HeaderAnalyzer::_limbo(unsigned char c)
// {
//     if (_is_vchar(c))
//         key.push_back
// }

void HeaderAnalyzer::_header_end_crlf(unsigned char c)
{
    if (c == '\n')
        _state = END_HEADERS;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            BAD_REQUEST_MSG);
}

// void HeaderAnalyzer::_header_end_lf(unsigned char c)
// {
//     (void)c;
// }

std::map<std::string, std::string> HeaderAnalyzer::headers()
{
    return (_map);
}

bool HeaderAnalyzer::done(void) const
{
    return (_state == END_HEADERS);
}

// std::string HeaderAnalyzer::host() const
// {
//     return (_host);
// }

// std::string HeaderAnalyzer::accept() const
// {
//     return (_accept);
// }

// std::string HeaderAnalyzer::accept_encoding() const
// {
//     return (_accept_encoding);
// }

// std::string HeaderAnalyzer::connection() const
// {
//     return (_connection);
// }

// ConnectionType HeaderAnalyzer::connection_type() const
// {
//     return (_connection_type);
// }

// std::string HeaderAnalyzer::content_length() const
// {
//     return (_content_length);
// }

} // namespace webshell
