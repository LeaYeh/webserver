/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:39 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/25 20:06:12 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "HeaderAnalyzer.hpp"
#include "Request.hpp"
#include "RequestLineAnalyzer.hpp"
#include "defines.hpp"
#include <string>

namespace webshell
{

class RequestAnalyzer
{
public:
    void feed(const char ch);
    bool is_complete(void) const;
    void reset(void);
    RequestAnalyzerState state(void) const;
    Request& request(void);

public:
    RequestAnalyzer();
    RequestAnalyzer(std::string* read_buffer);
    ~RequestAnalyzer();
    RequestAnalyzer(const RequestAnalyzer& other);
    RequestAnalyzer& operator=(const RequestAnalyzer& other);

private:
    RequestAnalyzerState _state;
    RequestLineAnalyzer _rl_analyzer;
    HeaderAnalyzer _header_analyzer;
    std::string* _read_buffer;
    RequestMethod _method;
    Uri _uri;
    float _version;
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _cookies;
    std::string _body;
    Request _req;

private:
    void _assemble_request(void);
    void _validate_method_and_uri();
};

} // namespace webshell

/*
RequestParser request_parser;

while (true)
{
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else
            throw std::runtime_error("read() failed: " +
std::string(strerror(errno)));
    }
    else if (bytes_read == 0)
    {
        break;
    }
    else
    {
        request_parser.read(buf, bytes_read);
        if (request_parser.is_complete())
        {
            request_parser.partial_body();
            break;
        }
    }
}

if (request_parser.is_complete())
{
    // Process the request
}
else
{
    // Wait for more data
}

// request_parser.reset();

*/
