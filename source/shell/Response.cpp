#include "Response.hpp"
#include "utils.hpp"
#include "shellUtils.hpp"

namespace webshell
{

Response::Response() : _status_code(OK), _headers(), _body()
{
}

Response::Response(const Response& other)
    : _status_code(other._status_code), _headers(other._headers),
      _body(other._body)
{
}

Response& Response::operator=(const Response& other)
{
    if (this != &other)
    {
        _status_code = other._status_code;
        _headers = other._headers;
        _body = other._body;
    }
    return (*this);
}

Response::~Response()
{
}

StatusCode Response::statusCode()
{
    return (_status_code);
}

std::string Response::header(std::string key)
{
    return (_headers[key]);
}

std::string Response::body()
{
    return (_body);
}

void Response::setStatusCode(StatusCode status_code)
{
    _status_code = status_code;
}

void Response::setHeader(std::string key, std::string value)
{
    _headers[key] = value;
}

void Response::setBody(std::string body)
{
    _body = body;
}

/*
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 13
Connection: close

Hello, World!
*/
std::string Response::serialize()
{
    std::string response;

    response += "HTTP/1.1 " + utils::toString(_status_code) + " " +
                statusReasonPhase(_status_code) + "\r\n";
    response += "Content-Type: text/plain\r\n";
    // response += "Content-Length: 13\r\n";
    response += "Connection: close\r\n\r\n";
    // response += "Hello, World!";

    return (response);
}

} // namespace webshell
