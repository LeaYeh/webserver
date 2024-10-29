#pragma once

#include "HeaderAnalyzer.hpp"
#include "Request.hpp"
#include "RequestLineAnalyzer.hpp"
#include "UriAnalyzer.hpp"
#include "defines.hpp"

#include <string>
#include <vector>

namespace webshell
{

class RequestAnalyzer
{
  public:

    RequestAnalyzer();
    ~RequestAnalyzer();
    RequestAnalyzer(const RequestAnalyzer& other);
    RequestAnalyzer& operator=(const RequestAnalyzer& other);

    void feed(const char ch);
    bool isComplete(void) const;
    void reset(void);
    RequestAnalyzerState state(void) const;
    Request request(void) const;

  private:


    RequestAnalyzerState _state;
    RequestLineAnalyzer _rl_analyzer;
    // UriAnalyzer _uri_analyzer; //Should be inside RL analyzer!!
    HeaderAnalyzer _header_analyzer;
    // BodyAnalyzer _body_analyzer;

    RequestMethod _method;
    std::vector<unsigned char> _target;
    float _version;
    //How i fill a map with headers from inside??
    //std::map<std::string, std::string> _headers;
    std::vector<unsigned char> _body;
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
