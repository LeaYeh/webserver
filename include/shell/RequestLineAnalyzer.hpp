#pragma once

#include "UriAnalyzer.hpp"
#include "URIMachine.hpp"
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
    std::string target() const;
    float version(void) const;
  
    void feed(unsigned char ch);
    bool done(void) const;
    void reset(void);

  private:

    StateMachine<RequestLineState> _machine;
    UriAnalyzer _uri_analyser;

    // RequestMethod _request_method;
    std::string _method;
    std::string _uri;
    std::string _version;
};

} // namespace webshell
