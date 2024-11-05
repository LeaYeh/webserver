/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLineAnalyzer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:52:31 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/05 17:06:52 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestLineAnalyzer.hpp"
#include "HttpException.hpp"
#include "defines.hpp"

#ifndef MACHINE_DEBUG
# define MACHINE_DEBUG 0
#endif

namespace webshell
{

static bool is_tchar(unsigned char c)
{
    if (isdigit(c) || isalpha(c))
        return (true);
    if (c == '!' || c == '#' || c == '$' || c == '%')
        return (true);
    if (c == '&' || c == '`' || c == '*' || c == '+')
        return (true);
    if (c == '-' || c == '.' || c == '^' || c == '_')
        return (true);
    if (c == '|' || c == '~') //TODO: ‘ is not even in ascii table
        return (true);
    return (false);
}

static void generic_transition_function(void)
{
    if (MACHINE_DEBUG)
        std::cerr << "moving states" << std::endl;
}

static bool validate_start(unsigned char c)
{
    if (!is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);
    else
        return (true);
}

static bool analyze_method(unsigned char c)
{
    if (c == ' ')
        return (true);
    else if (!is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);
    else
        return (false);
}

static bool analyze_uri(unsigned char c)
{
    if (c == ' ')
        return (true);
    else
        return (false);
}

static bool analyze_version(unsigned char c)
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
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

RequestLineAnalyzer::RequestLineAnalyzer()
{
    _machine.addState(RQLINE_START);
    _machine.addState(METHOD);
    _machine.addState(URI);
    _machine.addState(VERSION);
    _machine.addState(END_RQLINE);

    _machine.setFinalState(END_RQLINE);

    _machine.addTransition(RQLINE_START, METHOD,
                generic_transition_function);
    _machine.addTransition(METHOD, URI,
                generic_transition_function);
    _machine.addTransition(URI, VERSION,
                generic_transition_function);
    _machine.addTransition(VERSION, END_RQLINE,
                generic_transition_function);

    _machine.addAction(RQLINE_START, validate_start);
    _machine.addAction(METHOD, analyze_method);
    _machine.addAction(URI, analyze_uri);
    _machine.addAction(VERSION, analyze_version);
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _state(other._state), _uri_analyser(other._uri_analyser),
      _request_method(other._request_method), _method(other._method),
      _uri(other._uri), _version(other._version)
{
}

//TODO: make machine also copyable
RequestLineAnalyzer&
RequestLineAnalyzer::operator=(const RequestLineAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _uri_analyser = other._uri_analyser;
        _request_method = other._request_method;
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
    //TODO: handle HEAD and PUT here??
}

float RequestLineAnalyzer::version() const
{
    return (atof(_version.c_str()));
}

std::string RequestLineAnalyzer::target() const
{
    return (_uri);
}

void RequestLineAnalyzer::feed(unsigned char ch)
{
    switch (_machine.getCurrentState())
    {
        case RQLINE_START:
        {
            if (_machine.feed(ch))
            {
                _method.push_back(ch);
                _machine.transitionTo(METHOD);
            }
            break;
        }
        case METHOD:
        {
            if (_machine.feed(ch))
                _machine.transitionTo(URI);
            else
                _method.push_back(ch);
            break;
        }
        case URI:
        {
            if (_machine.feed(ch))
                _machine.transitionTo(VERSION);
            else
                _uri.push_back(ch);
            break;
        }
        case VERSION:
        {
            if (_machine.feed(ch))
                _machine.transitionTo(END_RQLINE);
            else if (ch != '\r')
                _method.push_back(ch);
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

void RequestLineAnalyzer::_parse_request_line(const std::string& line,
                                              RequestLineState current_state)
{
    (void)line;
    (void)current_state;
}

void RequestLineAnalyzer::_parse_method(const std::string& line)
{
    (void)line;
}

void RequestLineAnalyzer::_parse_version(const std::string& line)
{
    (void)line;
}

} // namespace webshell
