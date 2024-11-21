/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderFieldValidator.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 18:35:36 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/19 20:41:37 by mhuszar          ###   ########.fr       */
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

    void validate(std::map<std::string, std::string>& map);

private:

    void _validate_host(std::string& val);
    void _feed_hostname(unsigned char c);
    void _uri_host_regname(unsigned char c);
    void _uri_port(unsigned char c);
    bool _is_unreserved(unsigned char c);
    bool _is_sub_delim(unsigned char c);
    
    URIState _host_state;
};

} //namespace webshell