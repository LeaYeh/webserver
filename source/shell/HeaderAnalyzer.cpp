/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderAnalyzer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 18:50:44 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/18 12:59:05 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderAnalyzer.hpp"
#include "utils.hpp"

namespace webshell
{

HeaderAnalyzer::HeaderAnalyzer() : _connection_type(KEEP_ALIVE)
{
}

HeaderAnalyzer::HeaderAnalyzer(const HeaderAnalyzer& other)
    : _state(other._state), _host(other._host),
      _header_new_line(other._header_new_line), _host_name(other._host_name),
      _accept(other._accept), _accept_type(other._accept_type),
      _accept_encoding(other._accept_encoding),
      _accept_encoding_type(other._accept_encoding_type),
      _connection(other._connection), _connection_type(other._connection_type),
      _content_type(other._content_type),
      _content_type_name(other._content_type_name),
      _content_length(other._content_length),
      _content_length_nbr(other._content_length_nbr),
      _invalid_header(other._invalid_header), _the_rest(other._the_rest)
{
}

HeaderAnalyzer& HeaderAnalyzer::operator=(const HeaderAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _host = other._host;
        _header_new_line = other._header_new_line;
        _host_name = other._host_name;
        _accept = other._accept;
        _accept_type = other._accept_type;
        _accept_encoding = other._accept_encoding;
        _accept_encoding_type = other._accept_encoding_type;
        _connection = other._connection;
        _connection_type = other._connection_type;
        _content_type = other._content_type;
        _content_type_name = other._content_type_name;
        _content_length = other._content_length;
        _content_length_nbr = other._content_length_nbr;
        _invalid_header = other._invalid_header;
        _the_rest = other._the_rest;
    }
    return (*this);
}

HeaderAnalyzer::~HeaderAnalyzer()
{
}

void HeaderAnalyzer::parse_headers(const std::string& line,
                                   RequestHeaderState current_state)
{
    (void)line;
    (void)current_state;
}

void HeaderAnalyzer::feed(unsigned char c)
{
    switch (_state)
    {
        case START_HEADER:
            _start_header(c);
            break;
        case FIELD_NAME:
            _field_name(c);
            break;
        case LEADING_WS:
            _leading_ws(c);
        case FIELD_VALUE:
            _field_val(c);
            break;
        case MIDDLE_WS:
            _middle_ws(c);
        case TRAILING_WS:
            _trailing_ws(c);
        case FIELD_END_CR:
            _field_end_cr(c);
            break;
        case FIELD_END_LF:
            _field_end_lf(c);
            break;
        case HEADER_END_CR:
            _header_end_cr(c);
            break;
        case HEADER_END_LF:
            _header_end_lf(c);
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

void HeaderAnalyzer::_start_header(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_field_name(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_leading_ws(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_field_val(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_middle_ws(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_trailing_ws(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_field_end_cr(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_field_end_lf(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_header_end_cr(unsigned char c)
{
    (void)c;
}

void HeaderAnalyzer::_header_end_lf(unsigned char c)
{
    (void)c;
}

std::map<std::string, std::string> HeaderAnalyzer::headers()
{
    return (_headers);
}

bool HeaderAnalyzer::done(void) const
{
    // return (_state == END_HEADERS);
    return (true);
}

std::string HeaderAnalyzer::host() const
{
    return (_host);
}

std::string HeaderAnalyzer::accept() const
{
    return (_accept);
}

std::string HeaderAnalyzer::accept_encoding() const
{
    return (_accept_encoding);
}

std::string HeaderAnalyzer::connection() const
{
    return (_connection);
}

ConnectionType HeaderAnalyzer::connection_type() const
{
    return (_connection_type);
}

std::string HeaderAnalyzer::content_length() const
{
    return (_content_length);
}

} // namespace webshell
