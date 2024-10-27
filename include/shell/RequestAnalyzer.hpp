#pragma once

#include "HeaderAnalyzer.hpp"
#include "Request.hpp"
#include "RequestLineAnalyzer.hpp"
#include "UriAnalyzer.hpp"
#include "defines.hpp"

#include <string>

namespace webshell
{

class RequestAnalyzer
{
  public:
    // TODO: put the copy constructor and assignment operator in private
    RequestAnalyzer(const RequestAnalyzer& other);
    RequestAnalyzer& operator=(const RequestAnalyzer& other);

    RequestAnalyzer();
    ~RequestAnalyzer();

    void feed(const char ch);
    bool isComplete(void) const;
    void reset(void);
    RequestAnalyzerState state(void) const;
    Request request(void) const;

    std::pair<StatusCode, std::string> statusInfo();
    bool hasError();

  private:
    RequestAnalyzerState _state;
    RequestLineAnalyzer _request_line_analyzer;
    UriAnalyzer _uri_analyzer;
    HeaderAnalyzer _header_analyzer;
    std::pair<StatusCode, std::string> _status_info;
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
        request_parser.read(buf, bytes_read);
        if (request_parser.isComplete())
        {
            request_parser.partial_body();
            break;
        }
    }
}

if (request_parser.isComplete())
{
    // Process the request
}
else
{
    // Wait for more data
}

// request_parser.reset();

*/
