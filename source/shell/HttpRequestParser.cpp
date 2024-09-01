#include "HttpRequestParser.hpp"

namespace webshell
{

HttpRequestParser::HttpRequestParser() : _state(PARSING_REQUEST_LINE)
{
}

HttpRequestParser::HttpRequestParser(const HttpRequestParser& other)
    : _state(other._state),
      _request_line_analyzer(other._request_line_analyzer),
      _header_analyzer(other._header_analyzer),
      _partial_body(other._partial_body)
{
}

HttpRequestParser::~HttpRequestParser()
{
}

HttpRequestParser& HttpRequestParser::operator=(const HttpRequestParser& other)
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

void HttpRequestParser::update_state(const char* buf, size_t chunk_size)
{
    (void)buf;
    (void)chunk_size;
}

bool HttpRequestParser::is_complete(void) const
{
    return (true);
}

void HttpRequestParser::reset(void)
{
    _state = PARSING_REQUEST_LINE;
    _partial_body.clear();
}

std::string HttpRequestParser::partial_body(void) const
{
    return (_partial_body);
}

RequestAnalyzerState HttpRequestParser::state(void) const
{
    return (_state);
}

const RequestLineAnalyzer& HttpRequestParser::request_line_analyzer(void) const
{
    return (_request_line_analyzer);
}

const HeaderAnalyzer& HttpRequestParser::header_analyzer(void) const
{
    return (_header_analyzer);
}

} // namespace webshell
