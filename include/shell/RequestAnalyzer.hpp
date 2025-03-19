/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestAnalyzer.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:34:39 by mhuszar           #+#    #+#             */
/*   Updated: 2025/03/19 15:02:12 by mhuszar          ###   ########.fr       */
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
