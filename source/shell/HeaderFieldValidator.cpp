/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldValidator.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 18:42:39 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/25 20:29:37 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderFieldValidator.hpp"
#include "HttpException.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <cctype>
#include <cstddef>
#include <stdexcept>

namespace webshell
{

HeaderFieldValidator::HeaderFieldValidator()
{
    _host_state = URI_HOST_REGNAME;
    _cookie_state = CO_OWS_START;
}
HeaderFieldValidator::~HeaderFieldValidator() {}
HeaderFieldValidator::HeaderFieldValidator(const HeaderFieldValidator& other)
{
    _host_state = other._host_state;
    _cookie_state = other._cookie_state;
    _cookie_map = other._cookie_map;
}

HeaderFieldValidator&
HeaderFieldValidator::operator=(const HeaderFieldValidator& other)
{
    if (this != &other) {
        _host_state = other._host_state;
        _cookie_state = other._cookie_state;
        _cookie_map = other._cookie_map;
    }
    return (*this);
}

void HeaderFieldValidator::set_method(RequestMethod method)
{
    _method = method;
}

std::map<std::string, std::string> HeaderFieldValidator::get_cookie_map() const
{
    return (_cookie_map);
}

/*
Whitelist:
Host
Expect
// Range ?
// Authorization ?
// Accept-Encoding - we only support identity
Transfer-Encoding
Content-Length
*/

// Authorization  = credentials
// credentials    = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS "," OWS
// auth-param ) ] ) ] auth-scheme    = token token68        = 1*( ALPHA / DIGIT
// / "-" / "." / "_" / "~" / "+" / "/" ) *"=" auth-param     = token BWS "=" BWS
// ( token / quoted-string )

// Accept-Ranges: bytes
// Accept-Ranges: none

// Content-Length = 1*DIGIT
// Transfer-Encoding = #transfer-coding

/*
If a Transfer-Encoding header field
is present in a request and the chunked transfer coding is not
the final encoding, the message body length cannot be determined
reliably; the server MUST respond with the 400 (Bad Request)
status code and then close the connection.
*/

/*
A server that receives a request message with a transfer coding it
does not understand SHOULD respond with 501 (Not Implemented).
*/

/*
If a message is received with both a Transfer-Encoding and a
Content-Length header field, the Transfer-Encoding overrides the
Content-Length (RFC 7230 3.3.3)
*/

/*
A server MAY reject a request that contains a message body but not a
Content-Length by responding with 411 (Length Required).
*/

void HeaderFieldValidator::validate(std::map<std::string, std::string>& map)
{
    if (map.empty() || map.find("host") == map.end()) {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Host header field missing");
    }
    else {
        _validate_host(map["host"]);
    }
    if (map.find("expect") != map.end() && map["expect"] != "100-continue") {
        throw utils::HttpException(
            webshell::EXPECTATION_FAILED,
            "Only accepted Expect field value is \"100-continue\"");
    }
    if (map.find("cache-control") != map.end()) {
        _validate_cache_control(map["cache-control"]);
    }
    if (map.find("cookie") != map.end()) {
        _validate_cookie(map["cookie"]);
    }
    if (map.find("transfer-encoding") != map.end()) {
        if (map["transfer-encoding"] != "chunked") {
            throw utils::HttpException(
                webshell::NOT_IMPLEMENTED,
                "Only chunked transfer-coding is accepted");
        }
        if (map.find("content-length") != map.end()) {
            map.erase("content-length");
        }
    }
    else if (map.find("content-length") != map.end()) {
        _validate_content_length(map["content-length"]);
    }
    else if (_method == POST) {
        throw utils::HttpException(
            webshell::NO_CONTENT_LENGTH,
            "POST request needs to have Content-Length or Transfer-Encoding");
    }
}

void HeaderFieldValidator::_validate_content_length(std::string& val)
{
    size_t idx = 0;
    while (idx < val.size()) {
        if (!isdigit(val[idx])) {
            throw utils::HttpException(
                webshell::BAD_REQUEST,
                "Content length must be a decimal number greater than or equal to 0");
        }
        idx++;
    }
}

void HeaderFieldValidator::_validate_cookie(std::string& val)
{
    _cookie_state = CO_OWS_START;
    size_t len = val.size();
    size_t idx = 0;
    while (idx < len)
    {
        switch (_cookie_state)
        {
            case CO_OWS_START:
                _cookie_ows_start(val[idx]);
                break;
            case CO_NAME:
                _cookie_name(val[idx]);
                break;
            case CO_VALUE:
                _cookie_val(val[idx]);
                break;
            case CO_SP:
                _cookie_space(val[idx]);
                break;
            case CO_OWS_END:
                _cookie_ows_end(val[idx]);
                break;
            default:
                throw std::runtime_error("Invalid cookie state");
        }
        idx++;
    }
    if (_cookie_state == CO_VALUE)
    {
        _cookie_map[_name] = _val;
        _name.clear();
        _val.clear();
    }
    else if (_cookie_state != CO_OWS_END)
    {
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Incorrectly formatted Cookie header field");
    }
}

void HeaderFieldValidator::_cookie_ows_start(unsigned char c)
{
    if (utils::is_tchar(c))
    {
        _name.push_back(c);
        _cookie_state = CO_NAME;
    }
    else if (!_is_ows(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Cookie header field val should begin with token or OWS");
}

void HeaderFieldValidator::_cookie_ows_end(unsigned char c)
{
    if (!_is_ows(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Cookie header field val should end with nothing or OWS");
}

void HeaderFieldValidator::_cookie_name(unsigned char c)
{
    if (c == '=')
        _cookie_state = CO_VALUE;
    else if (utils::is_tchar(c))
        _name.push_back(c);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Cookie-name should be token");
}

void HeaderFieldValidator::_cookie_val(unsigned char c)
{
    if (c == ';' || _is_ows(c))
    {
        _cookie_map[_name] = _val;
        _name.clear();
        _val.clear();
        if (c == ';')
            _cookie_state = CO_SP;
        else
            _cookie_state = CO_OWS_END;
    }
    else if (_is_cookie_val(c))
        _val.push_back(c);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Invalid character in cookie-val");
}

void HeaderFieldValidator::_cookie_space(unsigned char c)
{
    if (_is_ows(c))
        _cookie_state = CO_NAME;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Semicolon should be followed by single SP");
}

void HeaderFieldValidator::_validate_cache_control(std::string& val)
{
    _cache_state = C_DIRECTIVE_START;
    size_t len = val.size();
    size_t idx = 0;
    while (idx < len) {
        switch (_cache_state) {
        case C_DIRECTIVE_START:
            _c_start(val[idx]);
            break;
        case C_DIRECTIVE:
            _c_directive(val[idx]);
            break;
        case C_ARGUMENT_START:
            _c_argument_start(val[idx]);
            break;
        case C_ARGUMENT:
            _c_argument(val[idx]);
            break;
        default:
            throw utils::HttpException(
                webshell::BAD_REQUEST,
                "Cache-Control header field value incorrect (switch case)");
        }
        idx++;
    }
    if (_cache_state == C_ARGUMENT_START) {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Empty Cache-Control argument after = sign");
    }
}

void HeaderFieldValidator::_c_start(unsigned char c)
{
    if (utils::is_tchar(c)) {
        _cache_state = C_DIRECTIVE;
    }
    else {
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Cache-Control directives should begin with tchar");
    }
}

void HeaderFieldValidator::_c_directive(unsigned char c)
{
    if (c == '=') {
        _cache_state = C_ARGUMENT_START;
    }
    else if (c == ',') {
        _cache_state = C_DIRECTIVE_START;
    }
    else if (utils::is_tchar(c)) {
        return;
    }
    else {
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Cache-Control directives should be made of tchar");
    }
}

void HeaderFieldValidator::_c_argument_start(unsigned char c)
{
    if (isdigit(c)) {
        _cache_state = C_ARGUMENT;
    }
    else {
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Cache-Control argument should indicate seconds");
    }
}

void HeaderFieldValidator::_c_argument(unsigned char c)
{
    if (c == ',') {
        _cache_state = C_DIRECTIVE_START;
    }
    else if (isdigit(c)) {
        return;
    }
    else {
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Cache-Control argument should represent seconds");
    }
}

void HeaderFieldValidator::_validate_host(std::string& val)
{
    int idx = 0;
    int size = val.size();
    while (idx < size) {
        _feed_hostname(val[idx]);
        idx++;
    }
    _host_state = URI_HOST_REGNAME;
}

void HeaderFieldValidator::_feed_hostname(unsigned char c)
{
    switch (_host_state) {
    case URI_HOST_REGNAME:
        _uri_host_regname(c);
        break;
    // TODO: can (should) we do IPV4 as well?
    case URI_PORT:
        _uri_port(c);
        break;
    default:
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Host header field value incorrect (switch case)");
    }
}

void HeaderFieldValidator::_uri_host_regname(unsigned char c)
{
    if (_is_unreserved(c) || _is_sub_delim(c)) {
        return;
    }
    else if (c == ':') {
        _host_state = URI_PORT;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Bad configuration of hostname field value");
    }
}

void HeaderFieldValidator::_uri_port(unsigned char c)
{
    if (isdigit(c)) {
        return;
    }
    else {
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Bad configuration of hostname port field value");
    }
}

// TODO: move the following 2 to utils. Duplicate version exists in UriAnalyzer.
bool HeaderFieldValidator::_is_unreserved(unsigned char c)
{
    if (isalpha(c)) {
        return (true);
    }
    if (isdigit(c)) {
        return (true);
    }
    if (c == '-' || c == '.' || c == '_' || c == '~') {
        return (true);
    }
    return (false);
}

bool HeaderFieldValidator::_is_sub_delim(unsigned char c)
{
    if (c == '!' || c == '$' || c == '&' || c == '\'') {
        return (true);
    }
    if (c == '(' || c == ')' || c == '*' || c == '+') {
        return (true);
    }
    if (c == ',' || c == ';' || c == '=') {
        return (true);
    }
    return (false);
}

bool HeaderFieldValidator::_is_ows(unsigned char c)
{
    if (c == ' ' || c == '\t') {
        return (true);
    }
    return (false);
}

bool HeaderFieldValidator::_is_cookie_val(unsigned char c)
{
    if ((c > 0 && c < 31) || c == 127 || c == '\"' || c == ','
        || c == ';' || c == '\\') {
        return (false);
    }
    return (true);
}

} // namespace webshell
