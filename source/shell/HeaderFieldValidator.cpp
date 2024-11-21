/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldValidator.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 18:42:39 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/19 20:35:20 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderFieldValidator.hpp"
#include "HttpException.hpp"

namespace webshell
{

HeaderFieldValidator::HeaderFieldValidator()
{
    _host_state = URI_HOST_REGNAME;
}
HeaderFieldValidator::~HeaderFieldValidator()
{
    
}
HeaderFieldValidator::HeaderFieldValidator(const HeaderFieldValidator& other)
{
    (void)other;
}

HeaderFieldValidator& HeaderFieldValidator::operator=(const HeaderFieldValidator& other)
{
    if (this == &other)
        return (*this);
    else
        return (*this);
}

void HeaderFieldValidator::validate(std::map<std::string, std::string>& map)
{
    if (map.empty() || map.find("host") == map.end())
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Host header field missing");
    else
        _validate_host(map["host"]);
    
}

void HeaderFieldValidator::_validate_host(std::string& val)
{
    int idx = 0;
    int size = val.size();
    while (idx < size)
    {
        _feed_hostname(val[idx]);
        idx++;
    }
    _host_state = URI_HOST_REGNAME;
}

void HeaderFieldValidator::_feed_hostname(unsigned char c)
{
    switch (_host_state)
    {
        case URI_HOST_REGNAME:
            _uri_host_regname(c);
            break;
        //TODO: can (should) we do IPV4 as well?
        case URI_PORT:
            _uri_port(c);
            break;
        default:
            throw utils::HttpException(webshell::BAD_REQUEST,
                "Host header field value incorrect (switch case)");
    }
}

void HeaderFieldValidator::_uri_host_regname(unsigned char c)
{
    if (_is_unreserved(c) || _is_sub_delim(c))
        return ;
    else if (c == ':')
        _host_state = URI_PORT;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Bad configuration of hostname field value");
}

void HeaderFieldValidator::_uri_port(unsigned char c)
{
    if (isdigit(c))
        return ;
    else
        throw utils::HttpException(webshell::BAD_REQUEST,
            "Bad configuration of hostname port field value");
}

//TODO: move the following 2 to utils. Duplicate version exists in UriAnalyzer.
bool HeaderFieldValidator::_is_unreserved(unsigned char c)
{
    if (isalpha(c))
        return (true);
    if (isdigit(c))
        return (true);
    if (c == '-' || c == '.' || c == '_' || c == '~')
        return (true);
    return (false);
}

bool HeaderFieldValidator::_is_sub_delim(unsigned char c)
{
    if (c == '!' || c == '$' || c == '&' || c == '\'')
        return (true);
    if (c == '(' || c == ')' || c == '*' || c == '+')
        return (true);
    if (c == ',' || c == ';' || c == '=')
        return (true);
    return (false);
}

} //namespace webshell

