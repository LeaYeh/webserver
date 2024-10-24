#pragma once
#include "defines.hpp"
#include <string>
#include <map>

namespace webshell
{

class Response
{
  public:
    Response();
    Response(const Response&);
    Response& operator=(const Response&);
    ~Response();

    StatusCode statusCode();
    std::string header(std::string key);
    std::string body();
    void setStatusCode(StatusCode statusCode);
    void setHeader(std::string key, std::string value);
    void setBody(std::string body);
    void setVersion(float version);
    std::string serialize();

  private:
    StatusCode _status_code;
    float _version;
    std::map<std::string, std::string> _headers;
    std::string _body;
};

} // namespace webshell