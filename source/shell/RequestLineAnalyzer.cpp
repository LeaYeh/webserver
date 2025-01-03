/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLineAnalyzer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:52:31 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/03 17:30:43 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestLineAnalyzer.hpp"
#include "HttpException.hpp"
#include "defines.hpp"
#include "utils.hpp"

#ifndef MACHINE_DEBUG
# define MACHINE_DEBUG 0
#endif

namespace webshell
{

bool RequestLineAnalyzer::_validate_start(unsigned char c)
{
    if (!utils::is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at validate start");
    else
        return (true);
}

bool RequestLineAnalyzer::_analyze_method(unsigned char c)
{
    if (c == ' ')
        return (true);
    else if (!utils::is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                "error at check method");
    else
        return (false);
}

bool RequestLineAnalyzer::_analyze_uri(unsigned char c)
{
    if (c == ' ')
        return (true);
    else
        return (false);
}

bool RequestLineAnalyzer::_check_lf(unsigned char c)
{
    if (c == '\n')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at check_lf");  
}

bool RequestLineAnalyzer::_analyze_version(unsigned char c)
{
    static int pos = 0;

    pos++;
    if (c == 'H' && pos == 1)
        return (false);
    else if (c == 'T' && (pos == 2 || pos == 3))
        return (false);
    else if (c == 'P' && pos == 4)
        return (false);
    else if (c == '/' && pos == 5)
        return (false);
    else if (isdigit(c) && (pos == 6 || pos == 8))
        return (false);
    else if (c == '.' && pos == 7)
        return (false);
    else if (c == '\r' && pos == 9)
        return (false);
    else if (c == '\n' && pos == 10)
        return (pos = 0, true);
    else
    {
        pos = 0;
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at check_version");    
    }
}

RequestLineAnalyzer::RequestLineAnalyzer() : _state(PRE_CR)
{
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _state(other._state), _uri_analyzer(other._uri_analyzer),
      _method(other._method), _uri(other._uri), _version(other._version)
{
}

RequestLineAnalyzer&
RequestLineAnalyzer::operator=(const RequestLineAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _uri_analyzer = other._uri_analyzer;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
    }
    return (*this);
}

RequestLineAnalyzer::~RequestLineAnalyzer()
{
}

RequestMethod RequestLineAnalyzer::method() const
{
    if (_method == "GET")
    {
        return (GET);
    }
    else if (_method == "POST")
    {
        return (POST);
    }
    else if (_method == "DELETE")
    {
        return (DELETE);
    }
    else
    {
        return (UNKNOWN);
    }
}

float RequestLineAnalyzer::version() const
{
    if (_version.size() < 3)
        return (0.0);
    else
        return (atof(_version.substr(_version.length() - 3, 3).c_str()));
}

Uri RequestLineAnalyzer::uri() const
{
    return (_uri_analyzer.take_uri());
}

void RequestLineAnalyzer::feed(unsigned char ch)
{   
    switch (_state)
    {
        case PRE_CR:
        {
            if (ch == '\r')
                _state = PRE_LF;
            else
            {
                _validate_start(ch);
                _method.push_back(ch);
                _state = METHOD;
            }
            break;
        }
        case PRE_LF:
        {
            _check_lf(ch);
            _state = PRE_CR;
            break;
        }
        case METHOD:
        {
            if (_analyze_method(ch))
                _state = URI;
            else
                _method.push_back(ch);
            break ;
        }
        case URI:
        {
            if (_analyze_uri(ch))
            {
                 _uri_analyzer.parse_uri(_uri);
                 _state = VERSION;
            }
            else
                _uri.push_back(ch);
            break ;
        }
        case VERSION:
        {
            if (_analyze_version(ch))
                _state = END_RQLINE;
            else if (ch != '\r')
                _version.push_back(ch);
            break;
        }
        default:
            throw std::runtime_error("Request Line parse error");
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
}

} // namespace webshell
