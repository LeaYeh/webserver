/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:34 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/25 20:10:00 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestAnalyzer.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <iostream>

namespace webshell
{

RequestAnalyzer::RequestAnalyzer() :
    _state(PARSING_REQUEST_LINE), _rl_analyzer(), _header_analyzer()
{
}

RequestAnalyzer::RequestAnalyzer(std::string* read_buffer) :
    _state(PARSING_REQUEST_LINE),
    _rl_analyzer(),
    _header_analyzer(),
    _read_buffer(read_buffer)
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other) :
    _state(other._state),
    _rl_analyzer(other._rl_analyzer),
    _header_analyzer(other._header_analyzer),
    _read_buffer(other._read_buffer),
    _method(other._method),
    _uri(other._uri),
    _version(other._version),
    _headers(other._headers),
    _cookies(other._cookies),
    _body(other._body),
    _req(other._req)
{
}

RequestAnalyzer::~RequestAnalyzer() {}

RequestAnalyzer& RequestAnalyzer::operator=(const RequestAnalyzer& other)
{
    if (this != &other) {
        _state = other._state;
        _rl_analyzer = other._rl_analyzer;
        _header_analyzer = other._header_analyzer;
        _read_buffer = other._read_buffer;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _cookies = other._cookies;
        _body = other._body;
        _req = other._req;
    }
    return (*this);
}

void RequestAnalyzer::feed(const char ch)
{
    switch (_state) {
    case PARSING_REQUEST_LINE:
        _rl_analyzer.feed(ch);
        if (_rl_analyzer.done()) {
            _state = PARSING_REQUEST_HEADERS;
            _method = _rl_analyzer.method();
            _header_analyzer.set_method(_method);
            _uri = _rl_analyzer.uri();
            _validate_method_and_uri();
            _version = _rl_analyzer.version();
        }
        break;
    case PARSING_REQUEST_HEADERS:
        _header_analyzer.feed(ch);
        if (_header_analyzer.done()) {
            _headers = _header_analyzer.headers();
            _cookies = _header_analyzer.cookies();
            _assemble_request();
            _state = COMPLETE;
        }
        break;
    default: {
        std::cerr << "State received: " << _state << std::endl;
        throw std::runtime_error("Request parse error");
    }
    }
}

void RequestAnalyzer::_validate_method_and_uri()
{
    if ((_method == CONNECT && _uri.type == AUTHORITY)
        || (_method == OPTIONS && _uri.type == ASTERISK)
        || _method == UNKNOWN) {
        throw utils::HttpException(
            webshell::NOT_IMPLEMENTED,
            "Only GET, POST and DELETE method supported");
    }
    else if (_uri.type == AUTHORITY || _uri.type == ASTERISK) {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Please use origin or absolute form");
    }
}

bool RequestAnalyzer::is_complete(void) const
{
    return (_state == COMPLETE);
}

void RequestAnalyzer::reset(void)
{
    _method = UNKNOWN;
    _uri.raw.clear();
    _uri.authority.clear();
    _uri.host.clear();
    _uri.port.clear();
    _uri.path.clear();
    _uri.query.clear();
    _uri.fragment.clear();
    _version = -0.0;
    _rl_analyzer.reset();
    _header_analyzer.reset();
    Request empty;
    _req = empty;
    _state = PARSING_REQUEST_LINE;
}

RequestAnalyzerState RequestAnalyzer::state(void) const
{
    return (_state);
}

Request& RequestAnalyzer::request(void)
{
    return (_req);
}

void RequestAnalyzer::_assemble_request()
{
    weblog::Logger::log(weblog::DEBUG,
                        "Assembling request struct Method: "
                            + utils::to_string(_method) + " Target: " + _uri.raw
                            + " Version: " + utils::to_string(_version));
    _req.set_method(_method);
    _req.set_uri(_uri);
    _req.set_version(_version);
    _req.set_headers(_headers);
    _req.set_reference(_read_buffer);
    // if (!_req.setupRequestConfig()) {
    //     throw utils::HttpException(webshell::NOT_FOUND,
    //                                "No matching location block found: "
    //                                    + _uri.path);
    // }
}

} // namespace webshell
