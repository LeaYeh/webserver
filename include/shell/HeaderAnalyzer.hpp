/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderAnalyzer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 18:50:54 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/27 17:49:23 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "AParser.hpp"
#include "HeaderFieldValidator.hpp"
#include "defines.hpp"
#include <map>

namespace webshell
{
class HeaderAnalyzer : public AParser
{
public:
    std::map<std::string, std::string> headers();
    std::map<std::string, std::string> cookies() const;
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
    std::map<std::string, std::string> _cookie_map;
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
};

} // namespace webshell
