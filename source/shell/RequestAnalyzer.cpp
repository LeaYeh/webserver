#include "RequestAnalyzer.hpp"

namespace webshell
{

RequestAnalyzer::RequestAnalyzer() : _state(PARSING_REQUEST_LINE)
{
}

RequestAnalyzer::RequestAnalyzer(const RequestAnalyzer& other)
    : _state(other._state),
      _request_line_analyzer(other._request_line_analyzer),
      _header_analyzer(other._header_analyzer)
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
        _header_analyzer = other._header_analyzer;
        _partial_body = other._partial_body;
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
    return (true);
}

void RequestAnalyzer::reset(void)
{
    _state = PARSING_REQUEST_LINE;
    _partial_body.clear();
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

} // namespace webshell
