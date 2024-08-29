#pragma once

#include "HeaderAnalyzer.hpp"
#include "RequestLineAnalyzer.hpp"
#include "defines.hpp"

#include <iostream>

namespace webshell
{

class HttpRequestParser
{
  public:
    HttpRequestParser();
    HttpRequestParser(const HttpRequestParser& other);
    HttpRequestParser& operator=(const HttpRequestParser& other);
    ~HttpRequestParser();

    void update_state(const char* buf, size_t chunk_size);
    bool is_complete(void) const;
    void reset(void);
    std::string partial_body(void) const;
    RequestAnalyzerState state(void) const;
    const RequestLineAnalyzer& request_line_analyzer(void) const;
    const HeaderAnalyzer& header_analyzer(void) const;

  private:
    RequestAnalyzerState _state;
    RequestLineAnalyzer _request_line_analyzer;
    HeaderAnalyzer _header_analyzer;
    std::string _partial_body;
};

} // namespace webshell

/*
RequestParser request_parser;

while (true)
{
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else
            throw std::runtime_error("read() failed: " +
std::string(strerror(errno)));
    }
    else if (bytes_read == 0)
    {
        break;
    }
    else
    {
        request_parser.update_state(buf, bytes_read);
        if (request_parser.is_complete())
        {
            request_parser.partial_body();
            break;
        }
    }
}

if (request_parser.is_complete())
{
    // Process the request
}
else
{
    // Wait for more data
}

// request_parser.reset();

*/
