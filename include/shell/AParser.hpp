/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 17:21:54 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/27 17:38:20 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace webshell
{

class AParser
{
public:

protected:
    AParser();
    virtual ~AParser();
    AParser(const AParser& other);
    AParser& operator=(const AParser& other);

protected:
    bool _is_unreserved(unsigned char c);
    bool _is_gen_delim(unsigned char c);
    bool _is_sub_delim(unsigned char c);
    bool _is_ows(unsigned char c);
    bool _is_tchar(unsigned char c);
    bool _is_vchar(unsigned char c);
    bool _is_pchar(unsigned char c);
    unsigned char _lowcase(unsigned char c);
    bool _is_query_or_fragment_part(unsigned char c);
    bool _is_cookie_val(unsigned char c);
};

} // namespace webshell