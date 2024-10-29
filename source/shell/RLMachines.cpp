/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RLMachines.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 21:10:19 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/01 18:24:24 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "defines.hpp"
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

VersionMachine::VersionMachine()
{
    
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


} // namespace webshell
