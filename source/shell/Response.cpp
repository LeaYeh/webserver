#include "Response.hpp"
#include "Logger.hpp"
#include "debugUtils.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"

namespace webshell
{

Response::Response() : _status_code(UNDEFINED), _headers(), _body() {}

Response::Response(const Response& other) :
    _status_code(other._status_code),
    _headers(other._headers),
    _body(other._body)
{
}

Response& Response::operator=(const Response& other)
{
    if (this != &other) {
        _status_code = other._status_code;
        _headers = other._headers;
        _body = other._body;
    }
    return (*this);
}

Response::~Response() {}

StatusCode Response::status_code()
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

void Response::set_status_code(StatusCode status_code)
{
    _status_code = status_code;
}

void Response::set_headers(std::map<std::string, std::string> headers)
{
    if (_headers.size() > 0) {
        LOG(weblog::WARNING, "Overwrite headers");
    }
    _headers = headers;
}

void Response::set_header(std::string key, std::string value)
{
    _headers[key] = value;
}

void Response::set_body(std::string body)
{
    _body = body;
    // _headers["Content-Length"] = utils::to_string(body.size());
}

void Response::clear_headers()
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
content-length: 13
Connection: close

Hello, World!
*/
std::string Response::serialize() const
{
    std::string response;
    std::map<std::string, std::string>::const_iterator it;

    if (_status_code == UNDEFINED) {
        LOG(weblog::WARNING, "Response status code is not set");
        return ("");
    }
    if (_status_code != IGNORE) {
        response += "HTTP/1.1 " + utils::to_string(_status_code) + " "
                    + status_reason_phase(_status_code) + "\r\n";
    }
    for (it = _headers.begin(); it != _headers.end(); ++it) {
        response += it->first + ": " + it->second + "\r\n";
    }
    if (_headers.size() > 0 && it == _headers.end()) {
        response += "\r\n";
    }
    response += _body;
    // if (_headers.find("transfer-encoding") == _headers.end()) {
    //     response += "\r\n";
    // }
    return (response);
}

} // namespace webshell
