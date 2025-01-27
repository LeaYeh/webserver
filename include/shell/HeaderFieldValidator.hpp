/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldValidator.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 18:35:36 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/27 17:48:12 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "AParser.hpp"
#include "defines.hpp"
#include <map>
#include <string>

namespace webshell
{

class HeaderFieldValidator : public AParser
{
public:
    void set_method(RequestMethod method);
    void validate(std::map<std::string, std::string>& map);
    std::map<std::string, std::string> get_cookie_map() const;

public:
    HeaderFieldValidator();
    ~HeaderFieldValidator();
    HeaderFieldValidator(const HeaderFieldValidator& other);
    HeaderFieldValidator& operator=(const HeaderFieldValidator& other);

private:
    URIState _host_state;
    CacheState _cache_state;
    CookieState _cookie_state;
    RequestMethod _method;

    std::string _name;
    std::string _val;
    std::map<std::string, std::string> _cookie_map;

private:
    void _validate_host(std::string& val);
    void _feed_hostname(unsigned char c);
    void _uri_host_regname(unsigned char c);
    void _uri_port(unsigned char c);

    void _validate_content_length(std::string& val);
    void _validate_cache_control(std::string& val);
    void _c_start(unsigned char c);
    void _c_directive(unsigned char c);
    void _c_argument_start(unsigned char c);
    void _c_argument(unsigned char c);

    void _validate_cookie(std::string& val);
    void _cookie_ows_start(unsigned char c);
    void _cookie_ows_end(unsigned char c);
    void _cookie_name(unsigned char c);
    void _cookie_val(unsigned char c);
    void _cookie_space(unsigned char c);
};

} // namespace webshell
