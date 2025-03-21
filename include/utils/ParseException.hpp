/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 12:32:03 by mhuszar           #+#    #+#             */
/*   Updated: 2025/03/19 15:12:04 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_EXCEPTION_HPP
#define PARSE_EXCEPTION_HPP

#include "defines.hpp"

class ParseException : public std::exception
{
public:
    void display_issue() const;
    webshell::StatusCode code() const;
    std::string msg() const;

public:
    ParseException(webshell::StatusCode code, std::string msg);
    ~ParseException() throw();
    ParseException(const ParseException& other);

private:
    webshell::StatusCode _code;
    std::string _msg;

private:
    ParseException(void);
    ParseException& operator=(const ParseException& other);
};

#endif
