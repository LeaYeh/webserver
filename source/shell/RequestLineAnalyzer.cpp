/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLineAnalyzer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:52:31 by mhuszar           #+#    #+#             */
/*   Updated: 2025/03/17 22:48:50 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestLineAnalyzer.hpp"
#include "HttpException.hpp"
#include "defines.hpp"
#include <stdexcept>

namespace webshell
{

RequestLineAnalyzer::RequestLineAnalyzer() : _state(PRE_CR), _pos(0) {}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other) :
    AParser(other),
    _state(other._state),
    _uri_analyzer(other._uri_analyzer),
    _method(other._method),
    _uri(other._uri),
    _version(other._version),
    _pos(other._pos)
{
}

RequestLineAnalyzer&
RequestLineAnalyzer::operator=(const RequestLineAnalyzer& other)
{
    if (this != &other) {
        _state = other._state;
        _uri_analyzer = other._uri_analyzer;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _pos = other._pos;
    }
    return (*this);
}

RequestLineAnalyzer::~RequestLineAnalyzer() {}

RequestMethod RequestLineAnalyzer::method() const
{
    if (_method == "GET") {
        return (GET);
    }
    else if (_method == "POST") {
        return (POST);
    }
    else if (_method == "DELETE") {
        return (DELETE);
    }
    else if (_method == "CONNECT") {
        return (CONNECT);
    }
    else if (_method == "OPTIONS") {
        return (OPTIONS);
    }
    else {
        return (UNKNOWN);
    }
}

float RequestLineAnalyzer::version() const
{
    if (_version.size() < 3) {
        return (0.0);
    }
    else {
        return (std::atof(_version.substr(_version.length() - 3, 3).c_str()));
    }
}

Uri RequestLineAnalyzer::uri() const
{
    return (_uri_analyzer.take_uri());
}

void RequestLineAnalyzer::feed(unsigned char ch)
{
    switch (_state) {
    case PRE_CR:
        _validate_start(ch);
        break;
    case PRE_LF:
        _check_lf(ch);
        break;
    case METHOD:
        _analyze_method(ch);
        break;
    case URI: {
        if (_collect_uri(ch)) {
            _uri_analyzer.parse_uri(_uri);
        }
        break;
    }
    case VERSION: {
        if (_analyze_version(ch)) {
            _state = END_RQLINE;
        }
        else if (ch != '\r') {
            _version.push_back(ch);
        }
        break;
    }
    default:
        throw std::runtime_error("Request Line parse error");
    }
}

void RequestLineAnalyzer::_validate_start(unsigned char c)
{
    if (c == '\r') {
        _state = PRE_LF;
    }
    else if (!_is_tchar(c)) {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "RLAnalyzer failed at validate start");
    }
    else {
        _method.push_back(c);
        _state = METHOD;
    }
}

void RequestLineAnalyzer::_check_lf(unsigned char c)
{
    if (c == '\n') {
        _state = PRE_CR;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "RLAnalyzer failed at check_lf");
    }
}

void RequestLineAnalyzer::_analyze_method(unsigned char c)
{
    if (_is_ows(c)) {
        _state = URI;
    }
    else if (!_is_tchar(c)) {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "error at check method");
    }
    else {
        _method.push_back(c);
    }
}

bool RequestLineAnalyzer::_collect_uri(unsigned char c)
{
    if (_is_ows(c)) {
        _state = VERSION;
        return (true);
    }
    else if (c == '\r' || c == '\n')
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "request-line incomplete");
    else {
        _uri.push_back(c);
        return (false);
    }
}

bool RequestLineAnalyzer::_analyze_version(unsigned char c)
{
    _pos++;
    if (c == 'H' && _pos == 1) {
        return (false);
    }
    else if (c == 'T' && (_pos == 2 || _pos == 3)) {
        return (false);
    }
    else if (c == 'P' && _pos == 4) {
        return (false);
    }
    else if (c == '/' && _pos == 5) {
        return (false);
    }
    else if (std::isdigit(c) && (_pos == 6 || _pos == 8)) {
        return (false);
    }
    else if (c == '.' && _pos == 7) {
        return (false);
    }
    else if (c == '\r' && _pos == 9) {
        return (false);
    }
    else if (c == '\n' && _pos == 10) {
        return (_pos = 0, true);
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "RLAnalyzer failed at check_version");
    }
}

bool RequestLineAnalyzer::done(void) const
{
    return (_state == END_RQLINE);
}

void RequestLineAnalyzer::reset(void)
{
    _state = PRE_CR;
    _uri_analyzer.reset();
    _method = "";
    _uri = "";
    _version = "";
    _pos = 0;
}

} // namespace webshell
