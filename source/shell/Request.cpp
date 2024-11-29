#include "Request.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "HttpException.hpp"
#include "utils.hpp"
#include <string>
#include <cstdlib>

namespace webshell
{

Request::Request()
    : _processed(0), _method(UNKNOWN), _uri(), _version(), _headers(), _read_buffer()
{
}

Request::Request(std::string* buffer)
    : _processed(0), _method(UNKNOWN), _uri(), _version(), _headers(), _read_buffer(buffer)
{
}

Request::Request(const Request& other)
    : _processed(other._processed), _method(other._method), _uri(other._uri), _version(other._version),
      _headers(other._headers), _read_buffer(other._read_buffer)
{
}

Request& Request::operator=(const Request& other)
{
    if (this != &other)
    {
        _processed = other._processed;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _read_buffer = other._read_buffer;
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

const std::string Request::serialize() const
{
    std::string serialized;

    // serialize request line
    serialized += requestMethodToString(_method) + " " + _uri.raw + " HTTP/" +
                  utils::toString(_version) + "\r\n";
    // serialize headers
    for (std::map<std::string, std::string>::const_iterator it =
             _headers.begin();
         it != _headers.end(); ++it)
        serialized += it->first + ": " + it->second + "\r\n";
    // serialize body
    serialized += "\r\n";

    return (serialized);
}

void Request::setMethod(RequestMethod method)
{
    _method = method;
}

void Request::setHeaders(std::map<std::string, std::string> headers)
{
    _headers = headers;
}

void Request::setUri(Uri uri)
{
    _uri = uri;
}

void Request::setVersion(float version)
{
    _version = version;
}

void Request::addHeader(std::string& name, std::string& value)
{
    _headers[name] = value;
}

bool Request::read_chunked_body(std::string& chunked_body)
{
    if (_headers.find("content-length") != _headers.end())
        return (_proceed_content_len(chunked_body));
    else
        return (_proceed_chunked(chunked_body));
}

bool Request::_proceed_content_len(std::string& chunked_body)
{
    static size_t maxlen = atoi(_headers["content-length"].c_str());
    static size_t chunksize = webkernel::CHUNKED_SIZE;

    if (maxlen - _processed > chunksize)
    {
        if ((*_read_buffer).size() < chunksize)
            throw utils::HttpException(webshell::BAD_REQUEST,
                "Mismatched Body Size");
        chunked_body = (*_read_buffer).substr(0, chunksize);
        (*_read_buffer).erase(0, chunksize);
        _processed += chunksize;
        return (false);
    }
    else
    {
        if ((*_read_buffer).size() < maxlen - _processed)
            throw utils::HttpException(webshell::BAD_REQUEST,
                "Mismatched Body Size");
        chunked_body = (*_read_buffer).substr(0, maxlen - _processed);
        (*_read_buffer).erase(0, maxlen - _processed);
        _processed = 0;
        return (true);
    }
}

bool Request::_proceed_chunked(std::string& chunked_body)
{
    (void)chunked_body;
    return(true);
}

// void Request::setBody(std::string& body)
// {
//     _body = body;
// }

} // namespace webshell
