#include "Request.hpp"
#include "defines.hpp"
#include <utility>

namespace webshell
{

Request::Request() : _method(UNKNOWN), _target(), _version(), _headers()/*, _body()*/
{
}

Request::Request(const Request& other)
    : _method(other._method), _target(other._target), _version(other._version),
      _headers(other._headers)/*, _body(other._body)*/
{
}

Request& Request::operator=(const Request& other)
{
    if (this != &other)
    {
        _method = other._method;
        _target = other._target;
        _version = other._version;
        _headers = other._headers;
        // _body = other._body;
    }
    return (*this);
}

Request::~Request()
{
}

RequestMethod Request::method()
{
    return (_method);
}

std::string Request::target()
{
    return (_target);
}

float Request::version()
{
    return (_version);
}

// std::string Request::body()
// {
//     return (_body);
// }

void Request::setMethod(RequestMethod method)
{
    _method = method;
}

void Request::setTarget(std::string target)
{
    _target = target;
}

void Request::setVersion(float version)
{
    _version = version;
}

// void Request::setBody(std::string& body)
// {
//     _body = body;
// }

} // namespace webshell
