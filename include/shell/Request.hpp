#pragma once
#include "defines.hpp"
#include <string>
#include <map>
#include <utility>

namespace webshell
{

class Request
{
  public:
    Request();
    Request(const Request&);
    //TODO: implement deep copy
    Request& operator=(const Request&);
    ~Request();

    RequestMethod method();
    std::string target();
    float version();
    // std::string body();

    void setMethod(RequestMethod method);
    void setTarget(std::string target);
    void setVersion(float version);
    // void setHeaders(std::map<std::string, std::string> headers);
    // void setBody(std::string& body);

  private:
    RequestMethod _method;
    std::string _target;
    float _version;
    std::map<std::string, std::string> _headers;
    // std::string _body;
};

} // namespace webshell
