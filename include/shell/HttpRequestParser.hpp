#pragma once

#include "HeaderAnalyzer.hpp"
#include "RequestLine.hpp"
#include "defines.hpp"

#include <iostream>

namespace webshell
{
/**
 * @brief Abstract class for parsing HTTP requests.
 *
 * The RequestParser class provides an interface for parsing HTTP requests.
 * It defines the parse function that must be implemented by derived classes.
 * Use state machine to parse the raw request.
 */
class HttpRequestParser
{
  public:
    /**
     * @brief Destroy the RequestParser object.
     */
    virtual ~HttpRequestParser();

    void update_state(const char* buf, size_t chunk_size);
    bool is_complete(void) const;
    void reset(void);
    void body(void);

  private:
    RequestLine _request_line;
    HeaderAnalyzer _header_analyzer;
    std::string _body;
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
