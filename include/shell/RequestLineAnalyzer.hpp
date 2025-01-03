#pragma once

#include "UriAnalyzer.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include <cstdlib>

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
    Uri uri() const;
    float version(void) const;
  
    void feed(unsigned char ch);
    bool done(void) const;
    void reset(void);

  private:

    bool _validate_start(unsigned char c);
    bool _analyze_method(unsigned char c);
    bool _analyze_uri(unsigned char c);
    bool _check_lf(unsigned char c);
    bool _analyze_version(unsigned char c);

    RequestLineState _state;
    UriAnalyzer _uri_analyzer;

    std::string _method;
    std::string _uri;
    std::string _version;
};

} // namespace webshell
