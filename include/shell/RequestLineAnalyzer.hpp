#pragma once

#include "UriAnalyzer.hpp"
#include "defines.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>

namespace webshell
{

class RequestLineAnalyzer
{
  public:
    RequestLineAnalyzer();
    RequestLineAnalyzer(const RequestLineAnalyzer& other);
    RequestLineAnalyzer& operator=(const RequestLineAnalyzer& other);
    ~RequestLineAnalyzer();

    RequestMethod method(void) const;
    std::vector<unsigned char> target() const;
    float version(void) const;
  
    void feed(unsigned char ch);
    bool done(void) const;

  private:
    void _parse_request_line(const std::string& line,
                             RequestLineState current_state);
    void _parse_method(const std::string& line);
    void _parse_version(const std::string& line);

    RequestLineState _state;
    UriAnalyzer _uri_analyser;
    RequestMethod _request_method;
    std::vector<unsigned char> _method;
    std::vector<unsigned char> _uri;

    // HttpVersion _http_version;
    std::vector<unsigned char> _version;
    // std::string _version_digit;
};

} // namespace webshell
