#include "Request.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <string>

namespace webshell
{

Request::Request()
    : _method(UNKNOWN), _uri(), _version(), _headers()/*, _body()*/
{
}

Request::Request(const Request& other)
    : _method(other._method), _uri(other._uri), _version(other._version),
      _headers(other._headers)/*, _body(other._body)*/
{
}

Request& Request::operator=(const Request& other)
{
    if (this != &other)
    {
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        // _body = other._body;
    }
    return (*this);
}

Request::~Request()
{
}

const RequestMethod& Request::method() const
{
    return (_method);
}

// const std::string Request::target() const
// {
//     return (_target);
// }

Uri Request::uri() const
{
    return (_uri);
}

float Request::version() const
{
    return (_version);
}

const std::map<std::string, std::string>& Request::headers() const
{
    return (_headers);
}

const std::string& Request::header(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(name);

    if (it == _headers.end())
        return (utils::EMPTY_STRING);
    return (it->second);
}

const std::string& Request::body() const
{
    return (_body);
}

const std::string Request::serialize() const
{
    std::string serialized;

    // serialize request line
    serialized += requestMethodToString(_method) + " " + _uri.raw +
                  " HTTP/" + utils::toString(_version) + "\r\n";
    // serialize headers
    for (std::map<std::string, std::string>::const_iterator it =
             _headers.begin();
         it != _headers.end(); ++it)
        serialized += it->first + ": " + it->second + "\r\n";
    // serialize body
    serialized += "\r\n" + _body;

    return (serialized);
}

void Request::setMethod(RequestMethod method)
{
    _method = method;
}

// void Request::setTarget(std::string target)
// {
//     _target = target;
// }

void Request::setUri(Uri uri)
{
    _uri = uri;
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
