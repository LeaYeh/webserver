/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:34 by mhuszar           #+#    #+#             */
/*   Updated: 2024/12/12 16:37:48 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestAnalyzer.hpp"
#include "HttpException.hpp"
#include "defines.hpp"

namespace webshell
{

RequestAnalyzer::RequestAnalyzer()
    : _state(PARSING_REQUEST_LINE), _rl_analyzer(), _header_analyzer()
{
}

RequestAnalyzer::RequestAnalyzer(int server_id, std::string* read_buffer)
    : _state(PARSING_REQUEST_LINE), _rl_analyzer(), _header_analyzer(),
      _server_id(server_id), _read_buffer(read_buffer)
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other)
    : _state(other._state), _rl_analyzer(other._rl_analyzer),
      _header_analyzer(other._header_analyzer), _server_id(other._server_id),
      _read_buffer(other._read_buffer)
{
}

RequestAnalyzer::~RequestAnalyzer()
{
}

RequestAnalyzer& RequestAnalyzer::operator=(const RequestAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _rl_analyzer = other._rl_analyzer;
        _header_analyzer = other._header_analyzer;
        _server_id = other._server_id;
        _read_buffer = other._read_buffer;
    }
    return (*this);
}

void RequestAnalyzer::feed(const char ch)
{
    switch (_state)
    {
    case PARSING_REQUEST_LINE:
        _rl_analyzer.feed(ch);
        if (_rl_analyzer.done())
        {
            _state = PARSING_REQUEST_HEADERS;
            _method = _rl_analyzer.method();
            _header_analyzer.set_method(_method);
            _uri = _rl_analyzer.uri();
            _version = _rl_analyzer.version();
        }
        break;
    case PARSING_REQUEST_HEADERS:
        _header_analyzer.feed(ch);
        if (_header_analyzer.done())
        {
            _headers = _header_analyzer.headers();
            _assemble_request();
            _state = COMPLETE;
        }
        break;
    default:
    {
        std::cerr << "State received: " << _state << std::endl;
        throw std::runtime_error("Request parse error");
    }
    }
}

bool RequestAnalyzer::isComplete(void) const
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

Request RequestAnalyzer::request(void) const
{
    return (_req);
}

void RequestAnalyzer::_assemble_request()
{
    std::cerr << "Assembling request struct. Method: " << _method
              << " Target: " << _uri.raw << " Version: " << _version
              << std::endl;
    _req.setMethod(_method);
    _req.setUri(_uri);
    _req.setVersion(_version);
    _req.setHeaders(_headers);
    _req.setReference(_read_buffer);
    if (!_req.setupRequestConfig(_server_id))
        throw utils::HttpException(webshell::NOT_FOUND,
                                   "No matching location block found: " +
                                       _uri.path);
}

} // namespace webshell
