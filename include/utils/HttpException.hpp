
#pragma once

#include "defines.hpp"
#include <exception>
#include <string>

namespace utils
{

class HttpException : public std::exception
{
  public:
    const char* what() const throw();
    webshell::StatusCode statusCode() const;
    std::string reasonDetail() const;

  public:
    HttpException(const webshell::StatusCode& statusCode,
                  const std::string& reasonDetail);
    virtual ~HttpException() throw();

  protected:
    webshell::StatusCode _statusCode;
    mutable std::string _reasonDetail;
};

} // namespace utils
