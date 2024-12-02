/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldValidator.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 18:35:36 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/30 21:38:09 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "defines.hpp"
#include <map>
#include <string>

namespace webshell
{

class HeaderFieldValidator
{
  public:

    HeaderFieldValidator();
    ~HeaderFieldValidator();
    HeaderFieldValidator(const HeaderFieldValidator& other);
    HeaderFieldValidator& operator=(const HeaderFieldValidator& other);

    void set_method(RequestMethod method);
    void validate(std::map<std::string, std::string>& map);

private:

    void _validate_host(std::string& val);
    void _feed_hostname(unsigned char c);
    void _uri_host_regname(unsigned char c);
    void _uri_port(unsigned char c);
    bool _is_unreserved(unsigned char c);
    bool _is_sub_delim(unsigned char c);

    void _validate_content_length(std::string& val);
    void _validate_cache_control(std::string& val);
    void _c_start(unsigned char c);
    void _c_directive(unsigned char c);
    void _c_argument_start(unsigned char c);
    void _c_argument(unsigned char c);
    
    URIState _host_state;
    CacheState _cache_state;

    RequestMethod _method;
};

} //namespace webshell