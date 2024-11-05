#include "RequestLineAnalyzer.hpp"

namespace webshell
{

RequestLineAnalyzer::RequestLineAnalyzer()
{
}

RequestLineAnalyzer::RequestLineAnalyzer(const RequestLineAnalyzer& other)
    : _state(other._state), _uri_analyser(other._uri_analyser),
      _request_method(other._request_method), _method(other._method),
      _uri(other._uri), _version(other._version)
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
        _version = other._version;
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
    return (atof(_version.c_str()));
}

std::string RequestLineAnalyzer::target() const
{
    return (_uri);
}
  
void RequestLineAnalyzer::feed(unsigned char ch)
{
    switch (_state)
        {
            case METHOD:
                _method_machine.feed(ch);
                if (_method_machine.done())
                    _state = URI;
                else
                    _method.push_back(ch);
                break;
            case URI:
                _uri_machine.feed(ch);
                if (_uri_machine.done())
                    _state = VERSION;
                else
                    _uri.push_back(ch);
                break;
            case VERSION:
                _version_machine.feed(ch);
                if (_version_machine.done())
                    _state = END_REQUEST_PARSER;
                else if (_version_machine.get_current_state() != RQLINE_CRLF)
                    _version.push_back(ch);
                break;
            default:
                throw std::runtime_error("Request Line parse error");
        }
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
