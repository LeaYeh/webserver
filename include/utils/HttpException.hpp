
#pragma once

#include "defines.hpp"
#include <exception>
#include <string>

namespace utils
{

class HttpException : public std::exception
{
  public:
    HttpException(const webshell::StatusCode& statusCode,
                  const std::string& reasonDetail);
    virtual ~HttpException() throw();

    const char* what() const throw();
    webshell::StatusCode statusCode() const;
    std::string reasonDetail() const;

  private:
    webshell::StatusCode _statusCode;
    mutable std::string _reasonDetail;
};

} // namespace utils
