/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderAnalyzer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 18:50:54 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/23 19:50:49 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HeaderFieldValidator.hpp"
#include "defines.hpp"
#include <map>

namespace webshell
{
class HeaderAnalyzer
{
public:
    std::map<std::string, std::string> headers();
    void set_method(RequestMethod method);

public:
    void feed(unsigned char ch);
    bool done(void) const;
    void reset(void);

public:
    HeaderAnalyzer();
    HeaderAnalyzer(const HeaderAnalyzer& other);
    HeaderAnalyzer& operator=(const HeaderAnalyzer& other);
    ~HeaderAnalyzer();

private:
    HeaderFieldValidator _validator;
    RequestHeaderState _state;
    std::map<std::string, std::string> _map;
    std::string _key;
    std::string _val;

private:
    void _start_header(unsigned char c);
    void _field_name(unsigned char c);
    void _leading_ws(unsigned char c);
    void _field_val(unsigned char c);
    void _middle_or_end_ws(unsigned char c);
    void _field_end_crlf(unsigned char c);
    void _check_obs_fold(unsigned char c);
    void _header_end_crlf(unsigned char c);
    bool _is_ows(unsigned char c);
    bool _is_vchar(unsigned char c);
    unsigned char _lowcase(unsigned char c);

    // TO_DO: Seperate the variables name with prefix by different usage?
    // std::string _host;
    // std::string _header_new_line;
    // std::string _host_name;
    // std::string _accept;
    // std::string _accept_type;
    // std::string _accept_encoding;
    // std::string _accept_encoding_type;
    // std::string _connection;
    // ConnectionType _connection_type;
    // std::string _content_type;
    // std::string _content_type_name;
    // std::string _content_length;
    // std::string _content_length_nbr;
    // std::string _invalid_header;
    // std::string _the_rest;
    // std::string host() const;
    // std::string accept() const;
    // std::string accept_encoding() const;
    // std::string connection() const;
    // ConnectionType connection_type() const;
    // std::string content_length() const;
};

} // namespace webshell
