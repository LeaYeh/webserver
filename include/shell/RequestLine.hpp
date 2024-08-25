#pragma once

#include "UriAnalyser.hpp"
#include "defines.hpp"
#include <iostream>

namespace webshell
{

class RequestLine
{
  public:
    RequestLine();
    ~RequestLine();

    std::string method(void) const;
    std::string version(void) const;

  private:
    void parse_request_line(const std::string& line,
                            RequestLineState currentState);
    void parse_method(const std::string& line);
    void parse_version(const std::string& line);

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
