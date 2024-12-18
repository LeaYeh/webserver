/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLineAnalyzer.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:52:31 by mhuszar           #+#    #+#             */
/*   Updated: 2024/12/03 21:11:11 by mhuszar          ###   ########.fr       */
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

// static bool is_tchar(unsigned char c)
// {
//     if (isdigit(c) || isalpha(c))
//         return (true);
//     if (c == '!' || c == '#' || c == '$' || c == '%')
//         return (true);
//     if (c == '&' || c == '`' || c == '*' || c == '+')
//         return (true);
//     if (c == '-' || c == '.' || c == '^' || c == '_')
//         return (true);
//     if (c == '|' || c == '~' || c == '\'')
//         return (true);
//     return (false);
// }

static void generic_transition_function(void)
{
    if (MACHINE_DEBUG)
        std::cerr << "moving states" << std::endl;
}

static bool validate_start(unsigned char c)
{
    if (!utils::is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at validate start");
    else
        return (true);
}

static bool analyze_method(unsigned char c)
{
    if (c == ' ')
        return (true);
    else if (!utils::is_tchar(c))
        throw utils::HttpException(webshell::BAD_REQUEST,
                "error at check method");
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

static bool check_lf(unsigned char c)
{
    if (c == '\n')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at check_lf");  
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
    {
        pos = 0;
        throw utils::HttpException(webshell::BAD_REQUEST,
                "RLAnalyzer failed at check_version");    
    }
}

RequestLineAnalyzer::RequestLineAnalyzer()
{
    _machine.addState(PRE_CR);
    _machine.addState(PRE_LF);
    // _machine.addState(RQLINE_START);
    _machine.addState(METHOD);
    _machine.addState(URI);
    _machine.addState(VERSION);
    _machine.addState(END_RQLINE);

    _machine.setFinalState(END_RQLINE);

    // _machine.addTransition(RQLINE_START, METHOD,
    //             generic_transition_function);
    _machine.addTransition(PRE_CR, METHOD,
                generic_transition_function);
    _machine.addTransition(PRE_CR, PRE_LF,
                generic_transition_function);
    _machine.addTransition(PRE_LF, PRE_CR,
                generic_transition_function);
    _machine.addTransition(METHOD, URI,
                generic_transition_function);
    _machine.addTransition(URI, VERSION,
                generic_transition_function);
    _machine.addTransition(VERSION, END_RQLINE,
                generic_transition_function);

    _machine.addAction(PRE_CR, validate_start);
    _machine.addAction(PRE_LF, check_lf);
    _machine.addAction(METHOD, analyze_method);
    _machine.addAction(URI, analyze_uri);
    _machine.addAction(VERSION, analyze_version);
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _machine(other._machine), _uri_analyzer(other._uri_analyzer),
      _method(other._method), _uri(other._uri), _version(other._version)
{
}

//TODO: make machine also copyable
RequestLineAnalyzer&
RequestLineAnalyzer::operator=(const RequestLineAnalyzer& other)
{
    if (this != &other)
    {
        _uri_analyzer = other._uri_analyzer;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _machine = other._machine;
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
    if (_version.size() < 3)
        return (0.0);
    else
        return (atof(_version.substr(_version.length() - 3, 3).c_str()));
}

// std::string RequestLineAnalyzer::target() const
// {
//     return (_uri);
// }

Uri RequestLineAnalyzer::uri() const
{
    return (_uri_analyzer.take_uri());
}

void RequestLineAnalyzer::feed(unsigned char ch)
{
    switch (_machine.getCurrentState())
    {
        case PRE_CR:
        {
            if (ch == '\r')
                _machine.transitionTo(PRE_LF);
            else
            {
                _machine.feed(ch);
                _method.push_back(ch);
                _machine.transitionTo(METHOD);
            }
            break;
        }
        case PRE_LF:
        {
            _machine.feed(ch);
            _machine.transitionTo(PRE_CR);
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
            {
                _uri_analyzer.parse_uri(_uri);
                _machine.transitionTo(VERSION);
            }
            else
                _uri.push_back(ch);
            break;
        }
        case VERSION:
        {
            if (_machine.feed(ch))
                _machine.transitionTo(END_RQLINE);
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
    return (_machine.getCurrentState() == END_RQLINE);
}

void RequestLineAnalyzer::reset(void)
{
    _machine.setCurrentState(PRE_CR);
    _uri_analyzer.reset();
    _method = "";
    _uri = "";
    _version = "";
}

} // namespace webshell
