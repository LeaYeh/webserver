#pragma once

#include "AParser.hpp"
#include "Uri.hpp"
#include "UriAnalyzer.hpp"
#include "defines.hpp"
#include <cstdlib>

namespace webshell
{

class RequestLineAnalyzer : public AParser
{
public:
    RequestMethod method(void) const;
    Uri uri() const;
    float version(void) const;
    void feed(unsigned char ch);
    bool done(void) const;
    void reset(void);

public:
    RequestLineAnalyzer();
    RequestLineAnalyzer(const RequestLineAnalyzer& other);
    RequestLineAnalyzer& operator=(const RequestLineAnalyzer& other);
    ~RequestLineAnalyzer();

private:
    RequestLineState _state;
    UriAnalyzer _uri_analyzer;

    std::string _method;
    std::string _uri;
    std::string _version;

private:
    void _validate_start(unsigned char c);
    void _check_lf(unsigned char c);
    void _analyze_method(unsigned char c);
    bool _collect_uri(unsigned char c);
    bool _analyze_version(unsigned char c);
};

} // namespace webshell
