/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:34 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/19 12:55:14 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestAnalyzer.hpp"
#include "defines.hpp"
#include "ParseException.hpp"

namespace webshell
{

RequestAnalyzer::RequestAnalyzer()
    : _state(PARSING_REQUEST_LINE), _rl_analyzer(),
      _header_analyzer()
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other)
    : _state(other._state),
      _rl_analyzer(other._rl_analyzer),
      _header_analyzer(other._header_analyzer)
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
                _uri = _rl_analyzer.uri();
                _version = _rl_analyzer.version();
            }
            break ;
        case PARSING_REQUEST_HEADERS:
            _header_analyzer.feed(ch);
            if (_header_analyzer.done())
            {
                _headers = _header_analyzer.headers();
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
    _state = PARSING_REQUEST_LINE;
}

RequestAnalyzerState RequestAnalyzer::state(void) const
{
    return (_state);
}

Request RequestAnalyzer::request(void) const
{
    std::cerr << "Request Line parsed. Method: " << _method << " Target: " << _uri.raw << " Version: " << _version << std::endl;
    Request req; 
    req.setMethod(_method);
    req.setUri(_uri);
    req.setVersion(_version);
    req.setHeaders(_headers);
    return (req);
}

} // namespace webshell
