/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:34 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/05 20:41:43 by mhuszar          ###   ########.fr       */
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
                _target = _rl_analyzer.target();
                _version = _rl_analyzer.version();
            }
            break ;
        case PARSING_REQUEST_HEADERS:
            _header_analyzer.feed(ch);
            if (_header_analyzer.done())
            {
                // _state = PARSING_REQUEST_BODY;
                //TODO: extract info from header here
                _state = COMPLETE;
            }
            break;
        default:
            throw std::runtime_error("Request parse error");
    }
    //TODO: how to handle body??
}

bool RequestAnalyzer::isComplete(void) const
{
    // TODO: check if received /r/n/r/n or 0 from chunked data or reached the
    // Content-Length or ...
    //MKH - i dont really think we need that
    return (_state == COMPLETE);
}

void RequestAnalyzer::reset(void)
{
    _state = PARSING_REQUEST_LINE;
}

RequestAnalyzerState RequestAnalyzer::state(void) const
{
    return (_state);
}

Request RequestAnalyzer::request(void) const
{
    std::cerr << "Request Line parsed. Method: " << _method << "Target: " << _target << "Version: " << _version << std::endl;
    Request req; 
    req.setMethod(_method);
    req.setTarget(_target);
    req.setVersion(_version);
    // req.setHeaders(_headers);
    return (req);
}

} // namespace webshell
