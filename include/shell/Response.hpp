#pragma once
#include "defines.hpp"
#include <map>
#include <string>

namespace webshell
{

class Response
{
public:
    StatusCode status_code();
    std::string header(std::string key);
    std::string body();
    void set_status_code(StatusCode status_code);
    void set_headers(std::map<std::string, std::string> headers);
    void set_header(std::string key, std::string value);
    void set_body(std::string body);
    void set_version(float version);
    void clear_headers();
    bool empty() const;
    std::string serialize() const;

public:
    Response();
    Response(const Response&);
    Response& operator=(const Response&);
    ~Response();

private:
    StatusCode _status_code;
    float _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

} // namespace webshell
