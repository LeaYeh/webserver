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

void RequestAnalyzer::read(const char* buf, size_t chunk_size)
{
    (void)buf;
    (void)chunk_size;
}

bool RequestAnalyzer::isComplete(void) const
{
    // TODO: check if received /r/n/r/n or 0 from chunked data or reached the Content-Length or ...
    return (true);
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
