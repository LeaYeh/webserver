/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseException.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 12:32:03 by mhuszar           #+#    #+#             */
/*   Updated: 2024/10/29 22:12:01 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_EXCEPTION_HPP
# define PARSE_EXCEPTION_HPP

#include "defines.hpp"
#include <iostream>

#define OK_MSG "OK"
#define CREATED_MSG "Created"
#define ACCEPTED_MSG "Accepted"
#define NO_CONTENT_MSG "No Content"
#define MOVED_PERMANENTLY_MSG "Moved Permanently"
#define FOUND_MSG "Found"
#define SEE_OTHER_MSG "See Other"
#define NOT_MODIFIED_MSG "Not Modified"
#define BAD_REQUEST_MSG "Bad Request"
#define UNAUTHORIZED_MSG "Unauthorized"
#define FORBIDDEN_MSG "Forbidden"
#define NOT_FOUND_MSG "Not Found"
#define METHOD_NOT_ALLOWED_MSG "Method Not Allowed"
#define REQUEST_TIMEOUT_MSG "Request Timeout"
#define NO_CONTENT_LENGTH_MSG "No Content Length"
#define INTERNAL_SERVER_ERROR_MSG "Internal Server Errror"
#define NOT_IMPLEMENTED_MSG "Not Implemented"
#define BAD_GATEWAY_MSG "Bad Gateway"
#define SERVICE_UNAVAILABLE_MSG "Service Unavaliable"


class ParseException : public std::exception
{
    public:

        ParseException(webshell::StatusCode code, std::string msg);
        ~ParseException() throw();
        ParseException(const ParseException& other);
    
        void displayIssue() const;
        webshell::StatusCode code() const;
        std::string msg() const;
        

    private:

        ParseException(void);
        ParseException& operator=(const ParseException& other);

        webshell::StatusCode _code;
        std::string _msg;
};

#endif