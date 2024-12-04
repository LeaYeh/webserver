#include "Response.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"

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

void Response::setHeaders(std::map<std::string, std::string> headers)
{
    if (_headers.size() > 0)
        weblog::Logger::log(weblog::WARNING, "Overwrite headers");
    _headers = headers;
}

void Response::setHeader(std::string key, std::string value)
{
    _headers[key] = value;
}

void Response::setBody(std::string body)
{
    _body = body;
    // _headers["Content-Length"] = utils::toString(body.size());
}

void Response::clearHeaders()
{
    _headers.clear();
}

bool Response::empty() const
{
    return (_status_code == UNDEFINED);
}

/*
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 13
Connection: close

Hello, World!
*/
std::string Response::serialize() const
{
    std::string response;

    if (_status_code != UNDEFINED)
        response += "HTTP/1.1 " + utils::toString(_status_code) + " " +
                    statusReasonPhase(_status_code) + "\r\n";
    std::map<std::string, std::string>::const_iterator it;
    for (it = _headers.begin(); it != _headers.end(); ++it)
    {
        response += it->first + ": " + it->second + "\r\n";
    }
    if (_headers.size() > 0 && it == _headers.end())
        response += "\r\n";
    response += _body;
    if (_headers.find("Transfer-Encoding") == _headers.end())
        response += "\r\n";

    return (response);
}

} // namespace webshell
