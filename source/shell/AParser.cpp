/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 17:30:25 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/27 17:44:26 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"
#include <cctype>

namespace webshell
{

AParser::AParser()
{
    
}
AParser::~AParser()
{
    
}
AParser::AParser(const AParser& other)
{
    (void)other;
}
AParser& AParser::operator=(const AParser& other)
{
    (void)other;
    return *this;
}

unsigned char AParser::_lowcase(unsigned char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (c += 32);
    }
    return (c);
}

bool AParser::_is_unreserved(unsigned char c)
{
    if (isalpha(c)) {
        return (true);
    }
    if (isdigit(c)) {
        return (true);
    }
    if (c == '-' || c == '.' || c == '_' || c == '~') {
        return (true);
    }
    return (false);
}

bool AParser::_is_sub_delim(unsigned char c)
{
    if (c == '!' || c == '$' || c == '&' || c == '\'') {
        return (true);
    }
    if (c == '(' || c == ')' || c == '*' || c == '+') {
        return (true);
    }
    if (c == ',' || c == ';' || c == '=') {
        return (true);
    }
    return (false);
}

bool AParser::_is_gen_delim(unsigned char c)
{
    if (c == ':' || c == '/' || c == '?' || c == '#') {
        return (true);
    }
    if (c == '[' || c == ']' || c == '@') {
        return (true);
    }
    return (false);
}

bool AParser::_is_pchar(unsigned char c)
{
    if (_is_unreserved(c)) {
        return (true);
    }
    if (_is_sub_delim(c)) {
        return (true);
    }
    if (c == ':') {
        return (true);
    }
    return (false);
}

bool AParser::_is_query_or_fragment_part(unsigned char c)
{
    if (_is_pchar(c)) {
        return (true);
    }
    if (c == '/' || c == '?') {
        return (true);
    }
    return (false);
}

bool AParser::_is_tchar(unsigned char c)
{
    if (isdigit(c) || isalpha(c)) {
        return (true);
    }
    if (c == '!' || c == '#' || c == '$' || c == '%') {
        return (true);
    }
    if (c == '&' || c == '`' || c == '*' || c == '+') {
        return (true);
    }
    if (c == '-' || c == '.' || c == '^' || c == '_') {
        return (true);
    }
    if (c == '|' || c == '~' || c == '\'') {
        return (true);
    }
    return (false);
}

bool AParser::_is_ows(unsigned char c)
{
    if (c == ' ' || c == '\t') {
        return (true);
    }
    return (false);
}

bool AParser::_is_vchar(unsigned char c)
{
    if (c > 32 && c < 127) { // 32 is technically printable but its part of ows
        return (true);
    }
    return (false);
}

bool AParser::_is_cookie_val(unsigned char c)
{
    if ((c > 0 && c < 31) || c == 127 || c == '\"' || c == ','
        || c == ';' || c == '\\') {
        return (false);
    }
    return (true);
}
    
}