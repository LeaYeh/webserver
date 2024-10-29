#include "RequestLineAnalyzer.hpp"

namespace webshell
{

RequestLineAnalyzer::RequestLineAnalyzer()
{
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _state(other._state), _uri_analyser(other._uri_analyser),
      _request_method(other._request_method), _method(other._method),
      _uri(other._uri), /*_http_version(other._http_version),*/
      _version(other._version)/*, _version_digit(other._version_digit)*/
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
        // _http_version = other._http_version;
        _version = other._version;
        // _version_digit = other._version_digit;
    }
    return (*this);
}

RequestLineAnalyzer::~RequestLineAnalyzer()
{
}

RequestMethod RequestLineAnalyzer::method() const
{
    if (_method == "GET")
    {
        return (GET);
    }
    else if (_method == "POST")
    {
        return (POST);
    }
    else if (_method == "DELETE")
    {
        return (DELETE);
    }
    else
    {
        return (UNKNOWN);
    }
    //TODO: handle HEAD and PUT here??
}

float RequestLineAnalyzer::version() const
{
    // unsigned char *str;
    // _version.push_back('\0');
    // str = reinterpret_cast<unsigned char*>(_version.data());
    //TODO: would this leak? Do i need to free?
    return (atof(_version.c_str()));
}

std::string RequestLineAnalyzer::target() const
{
    return (_uri);
}
  
void RequestLineAnalyzer::feed(unsigned char ch)
{
    (void)ch;
}
bool RequestLineAnalyzer::done(void) const
{
    return (_state == END_REQUEST_PARSER);
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
