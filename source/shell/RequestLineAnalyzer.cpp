#include "RequestLineAnalyzer.hpp"

namespace webshell
{

RequestLineAnalyzer::RequestLineAnalyzer()
{
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _state(other._state), _uri_analyser(other._uri_analyser),
      _request_method(other._request_method), _method(other._method),
      _uri(other._uri), _http_version(other._http_version),
      _version(other._version), _version_digit(other._version_digit)
{
}

RequestLineAnalyzer&
RequestLineAnalyzer::operator=(const RequestLineAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _uri_analyser = other._uri_analyser;
        _request_method = other._request_method;
        _method = other._method;
        _uri = other._uri;
        _http_version = other._http_version;
        _version = other._version;
        _version_digit = other._version_digit;
    }
    return (*this);
}

RequestLineAnalyzer::~RequestLineAnalyzer()
{
}

std::string RequestLineAnalyzer::method() const
{
    return (_method);
}

std::string RequestLineAnalyzer::version() const
{
    return (_version);
}

void RequestLineAnalyzer::_parse_request_line(const std::string& line,
                                              RequestLineState current_state)
{
    (void)line;
    (void)current_state;
}

void RequestLineAnalyzer::_parse_method(const std::string& line)
{
    (void)line;
}

void RequestLineAnalyzer::_parse_version(const std::string& line)
{
    (void)line;
}

} // namespace webshell
