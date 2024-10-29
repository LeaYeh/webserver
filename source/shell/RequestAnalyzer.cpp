#include "RequestAnalyzer.hpp"
#include "defines.hpp"

namespace webshell
{

RequestAnalyzer::RequestAnalyzer()
    : _state(PARSING_REQUEST_LINE), _request_line_analyzer(), _uri_analyzer(),
      _header_analyzer(), _status_info(OK, "")
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other)
    : _state(other._state),
      _request_line_analyzer(other._request_line_analyzer),
      _uri_analyzer(other._uri_analyzer),
      _header_analyzer(other._header_analyzer), _status_info(other._status_info)
{
}

RequestAnalyzer::~RequestAnalyzer()
{
}

RequestAnalyzer& RequestAnalyzer::operator=(const RequestAnalyzer& other)
{
    if (this != &other)
    {
        _state = other._state;
        _request_line_analyzer = other._request_line_analyzer;
        _uri_analyzer = other._uri_analyzer;
        _header_analyzer = other._header_analyzer;
        _status_info = other._status_info;
    }
    return (*this);
}

void RequestAnalyzer::feed(const char ch)
{
    // (void)ch;
    switch (_state)
    {
        case PARSING_REQUEST_LINE:
            if (_rl_analyzer.done())
            {
                _state = PARSING_REQUEST_HEADERS;
                _method = _rl_analyzer.method();
                _target = _rl_analyzer.target();
                _version = _rl_analyzer.version();
            }
            else
            {
                _rl_analyzer.feed(ch);
                break;
            }
        case PARSING_REQUEST_HEADERS:
            if (_header_analyzer.done())
            {
                //extract info from header here
                _state = PARSING_REQUEST_BODY;
            }
            else
            {
                _header_analyzer.feed(ch);
                break;
            }
        //TODO: i cant do this lol what is the point of chunked then
        case PARSING_REQUEST_BODY:
            if (_body_analyzer.done())
            {
                //extract info from header here
                _state = COMPLETE;
            }
            else
            {
                _body_analyzer.feed(ch);
                break;
            }
    }
}

bool RequestAnalyzer::isComplete(void) const
{
    // TODO: check if received /r/n/r/n or 0 from chunked data or reached the Content-Length or ...
    //MKH - i dont really think we need that
    return (_state = COMPLETE);
}

void RequestAnalyzer::reset(void)
{
    _state = PARSING_REQUEST_LINE;
}

RequestAnalyzerState RequestAnalyzer::state(void) const
{
    return (_state);
}

Request RequestAnalyzer::request(void) const
{
    Request req;
    req.setMethod(_method);
    req.setTarget(_target);
    req.setVersion(_version);
    req.setHeaders(_headers);
    return (req);
}

std::pair<StatusCode, std::string> RequestAnalyzer::statusInfo()
{
    return (_status_info);
}

bool RequestAnalyzer::hasError()
{
    return (_status_info.first != OK);
}

} // namespace webshell
