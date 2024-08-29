#pragma once

#include "UriAnalyser.hpp"
#include "defines.hpp"
#include <iostream>

namespace webshell
{

class RequestLineAnalyzer
{
  public:
    RequestLineAnalyzer();
    RequestLineAnalyzer(const RequestLineAnalyzer& other);
    RequestLineAnalyzer& operator=(const RequestLineAnalyzer& other);
    ~RequestLineAnalyzer();

    std::string method(void) const;
    std::string version(void) const;

  private:
    // TODO: Private functions and varible should be named with _ prefix.
    void _parse_request_line(const std::string& line,
                            RequestLineState current_state);
    void _parse_method(const std::string& line);
    void _parse_version(const std::string& line);

    RequestLineState _state;
    UriAnalyser _uri_analyser;
    RequestMethod _request_method;
    std::string _method;
    std::string _uri;

    HttpVersion _http_version;
    std::string _version;
    std::string _version_digit;
};

} // namespace webshell
