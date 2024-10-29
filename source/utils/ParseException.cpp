/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseException.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 12:32:49 by mhuszar           #+#    #+#             */
/*   Updated: 2024/10/29 21:58:26 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseException.hpp"

ParseException::ParseException(const ParseException& other)
{
    (void)other;
}

ParseException::ParseException(webshell::StatusCode code, std::string msg)
{
    _code = code;
    _msg = msg;
}

void ParseException::displayIssue() const
{
    std::cerr << "Parsing went wrong" << std::endl;
}

webshell::StatusCode ParseException::code() const
{
    return (_code);
}

std::string ParseException::msg() const
{
    return (_msg);
}

ParseException::~ParseException() throw() {}