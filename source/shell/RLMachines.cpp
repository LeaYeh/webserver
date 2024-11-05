/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RLMachines.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 21:10:19 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/05 15:08:29 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "defines.hpp"
#include "HttpException.hpp"
#include "RLMachines.hpp"
#include "ParseException.hpp"


namespace webshell
{
    
bool is_tchar(unsigned char c)
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

void generic_transition_function(void)
{
    if (MACHINE_DEBUG)
        std::cerr << "moving states" << std::endl;
}

bool analyze_method(unsigned char c)
{
    if (c == ' ')
        return (true);
    else if (!is_tchar(c))
        throw (ParseException(BAD_REQUEST, BAD_REQUEST_MSG));
    else
        return (false);
}

bool validate_start(unsigned char c)
{
    if (!is_tchar(c))
        throw (ParseException(BAD_REQUEST, BAD_REQUEST_MSG));
    else
        return (true);
}

MethodMachine::MethodMachine()
{
    _machine.addState(START);
    _machine.addState(METHOD);
    _machine.addState(SPACE_BEFORE_URI);
    _machine.setFinalState(SPACE_BEFORE_URI);
    _machine.addTransition(START, METHOD,
                generic_transition_function);
    _machine.addTransition(METHOD, SPACE_BEFORE_URI,
                generic_transition_function);
    _machine.addAction(START, validate_start);
    _machine.addAction(METHOD, analyze_method);
}

MethodMachine::~MethodMachine() {}

void MethodMachine::feed(unsigned char c)
{
    if (_machine.feed(c))
    {
        switch (_machine.getCurrentState())
        {
            case START:
                _machine.transitionTo(METHOD);
                break;
            case METHOD:
                _machine.transitionTo(SPACE_BEFORE_URI);
                break;
            default:
                break;
        }
    }
}

bool MethodMachine::done()
{
    return (_machine.done());
}

URIMachine::URIMachine()
{
    
}

URIMachine::~URIMachine() {}

void URIMachine::feed(unsigned char c)
{
    (void)c;
}

bool URIMachine::done()
{
    return (_machine.done());
}

static bool parse_h(unsigned char c)
{
    if (c == 'H')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_ts(unsigned char c)
{
    static int ctr = 1;
    
    if (c == 'T' && ctr++ == 1)
        return (false);
    else if (c == 'T' && ctr == 2)
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_p(unsigned char c)
{
    if (c == 'P')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_slash(unsigned char c)
{
    if (c == '/')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_digit(unsigned char c)
{
    if (isdigit(c))
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_dot(unsigned char c)
{
    if (c == '.')
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

static bool parse_crlf(unsigned char c)
{
    static int ctr = 1;
    
    if (c == '\r' && ctr++ == 1)
        return (false);
    else if (c == '\n' && ctr == 2)
        return (true);
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
                BAD_REQUEST_MSG);    
}

VersionMachine::VersionMachine()
{
    _machine.addState(VERSION_H);
    _machine.addState(VERSION_T);
    // _machine.addState(VERSION_T2);
    _machine.addState(VERSION_P);
    _machine.addState(VERSION_SLASH);
    _machine.addState(VERSION_MAJOR);
    _machine.addState(VERSION_DOT);
    _machine.addState(VERSION_MINOR);
    _machine.addState(RQLINE_CRLF);

    _machine.addTransition(VERSION_H, VERSION_T,
                generic_transition_function);
    _machine.addTransition(VERSION_T, VERSION_P,
                generic_transition_function);
    _machine.addTransition(VERSION_P, VERSION_SLASH,
                generic_transition_function);
    _machine.addTransition(VERSION_SLASH, VERSION_MAJOR,
                generic_transition_function);
    _machine.addTransition(VERSION_MAJOR, VERSION_DOT,
                generic_transition_function);
    _machine.addTransition(VERSION_DOT, VERSION_MINOR,
                generic_transition_function);
    _machine.addTransition(VERSION_MINOR, RQLINE_CRLF,
                generic_transition_function);
    
    _machine.addAction(VERSION_H, parse_h);
    _machine.addAction(VERSION_T, parse_ts);
    _machine.addAction(VERSION_P, parse_p);
    _machine.addAction(VERSION_SLASH, parse_slash);
    _machine.addAction(VERSION_MAJOR, parse_digit);
    _machine.addAction(VERSION_DOT, parse_dot);
    _machine.addAction(VERSION_MINOR, parse_digit);
    _machine.addAction(RQLINE_CRLF, parse_crlf);
}

VersionMachine::~VersionMachine() {}

void VersionMachine::feed(unsigned char c)
{
    (void)c;
}

bool VersionMachine::done()
{
    return (_machine.done());
}

VersionState VersionMachine::get_current_state(void)
{
    return (_machine.getCurrentState());
}


} // namespace webshell
