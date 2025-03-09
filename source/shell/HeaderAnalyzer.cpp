/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderAnalyzer.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 18:50:44 by mhuszar           #+#    #+#             */
/*   Updated: 2025/03/08 22:17:41 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderAnalyzer.hpp"
#include "HttpException.hpp"
#include "defines.hpp"
#include <stdexcept>

namespace webshell
{

HeaderAnalyzer::HeaderAnalyzer() /* : _connection_type(KEEP_ALIVE)*/
{
    _state = START_HEADER;
    _key = "";
    _val = "";
    _escape = false;
}

HeaderAnalyzer::HeaderAnalyzer(const HeaderAnalyzer& other) :
    AParser(other),
    _validator(other._validator),
    _state(other._state),
    _map(other._map),
    _cookie_map(other._cookie_map),
    _key(other._key),
    _val(other._val),
    _escape(other._escape)
{
}

HeaderAnalyzer& HeaderAnalyzer::operator=(const HeaderAnalyzer& other)
{
    if (this != &other) {
        _state = other._state;
        _map = other._map;
        _cookie_map = other._cookie_map;
        _key = other._key;
        _val = other._val;
        _escape = other._escape;
    }
    return (*this);
}

HeaderAnalyzer::~HeaderAnalyzer() {}

void HeaderAnalyzer::reset()
{
    _state = START_HEADER;
    _key.clear();
    _val.clear();
    _map.clear();
    _escape = false;
}

void HeaderAnalyzer::feed(unsigned char c)
{
    switch (_state) {
    case START_HEADER:
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
    case CHECK_OBS_FOLD:
        _check_obs_fold(c);
        break;
    case HEADER_END_CRLF:
        _header_end_crlf(c);
        break;
    case QUOTED:
        _quoted(c);
        break;
    default:
        throw std::runtime_error("Header analyzing went wrong");
    }
    _escape = (c == '\\' && !_escape);
}

void HeaderAnalyzer::set_method(RequestMethod method)
{
    _validator.set_method(method);
}

void HeaderAnalyzer::_start_header(unsigned char c)
{
    if (_is_tchar(c)) {
        _key.push_back(_lowcase(c));
        _state = FIELD_NAME;
    }
    else if (c == '\r') {
        _state = HEADER_END_CRLF;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST, "start header error");
    }
}

void HeaderAnalyzer::_field_name(unsigned char c)
{
    if (_is_tchar(c)) {
        _key.push_back(_lowcase(c));
    }
    else if (c == ':') {
        _state = LEADING_WS;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header fieldname state error");
    }
}

void HeaderAnalyzer::_leading_ws(unsigned char c)
{
    if (c == '\"')
        _state = QUOTED;
    else if (_is_ows(c)) {
        return;
    }
    else if (_is_vchar(c)) {
        _val.push_back(_lowcase(c));
        _state = FIELD_VALUE;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header leading_ws state error");
    }
}

void HeaderAnalyzer::_field_val(unsigned char c)
{
    if (c == '\"')
        _state = QUOTED;
    else if (_is_vchar(c)) {
        _val.push_back(_lowcase(c));
    }
    else if (_is_ows(c)) {
        _state = MIDDLE_OR_END_WS;
    }
    else if (c == '\r') {
        _state = FIELD_END_CRLF;
    }
    else {
        // std::cerr << "CHAR IS: |" << c << "| " << std::endl;
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header fieldval state error");
    }
}

void HeaderAnalyzer::_middle_or_end_ws(unsigned char c)
{
    if (c == '\"')
        _state = QUOTED;
    else if (_is_vchar(c)) {
        _val.push_back(' '); // should i separate like this or not at all?
        _val.push_back(_lowcase(c));
        _state = FIELD_VALUE;
    }
    else if (_is_ows(c)) {
        return;
    }
    else if (c == '\r') {
        _state = FIELD_END_CRLF;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header midws state error");
    }
}

void HeaderAnalyzer::_quoted(unsigned char c)
{
    if (c == '\\' && !_escape)
        return ;
    if (c == '\"' && !_escape)
        _state = MIDDLE_OR_END_WS;
    else if (_is_vchar(c) || _is_ows(c)) {
        _val.push_back(_lowcase(c));
    }
    else if (c == '\r') {
        _state = FIELD_END_CRLF;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "invalid character in quoted string");
    }
}

/*
If a message is received without Transfer-Encoding and with
either multiple Content-Length header fields having differing
field-values or a single Content-Length header field having an
invalid value, then the message framing is invalid and the
recipient MUST treat it as an unrecoverable error.  If this is a
request message, the server MUST respond with a 400 (Bad Request)
status code and then close the connection (RFC 7230 3.3.3).
*/

// TODO: this check fails if they provide multiple Content-Length fields
// first and *then* a transfer-encoding

void HeaderAnalyzer::_field_end_crlf(unsigned char c)
{
    // std::cerr << "Entred field_end crlf" << std::endl;
    // sleep(5);
    if (c == '\n') {
        if (_key == "host" && _map.find(_key) != _map.end()) {
            throw utils::HttpException(
                webshell::BAD_REQUEST,
                "multiple Host header fields are not allowed");
        }
        else if (_key == "content-length"
                 && _map.find("transfer-encoding") == _map.end()
                 && _map.find(_key) != _map.end() && _map[_key] != _val) {
            throw utils::HttpException(
                webshell::BAD_REQUEST,
                "multiple Content-Length header fields with differing value are not allowed");
        }
        _map[_key] = _val;
        _key.clear();
        _val.clear();
        _state = CHECK_OBS_FOLD;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header field_end_crfl state error");
    }
}

void HeaderAnalyzer::_check_obs_fold(unsigned char c)
{
    if (_is_tchar(c)) {
        _key.push_back(_lowcase(c));
        _state = FIELD_NAME;
    }
    else if (c == '\r') {
        _state = HEADER_END_CRLF;
    }
    else if (_is_ows(c)) {
        throw utils::HttpException(webshell::BAD_REQUEST,
        "Obs-fold detected.\nHeader field values spanning multiple lines are unacceptable.\nRefer to RFC 7230 3.2.4. for details.");
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Failed at obsfold check");
    }
}

void HeaderAnalyzer::_header_end_crlf(unsigned char c)
{
    if (c == '\n') {
        _validator.validate(_map);
        _cookie_map = _validator.get_cookie_map();
        _state = END_HEADERS;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "header end_crlf state error");
    }
}

std::map<std::string, std::string> HeaderAnalyzer::headers()
{
    return (_map);
}

std::map<std::string, std::string> HeaderAnalyzer::cookies() const
{
    return (_cookie_map);
}

bool HeaderAnalyzer::done(void) const
{
    return (_state == END_HEADERS);
}

} // namespace webshell
