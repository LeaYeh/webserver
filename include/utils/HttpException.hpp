
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
    webshell::ContentType contentType() const;

  public:
    HttpException(const webshell::StatusCode& statusCode,
                  const std::string& reasonDetail,
                  webshell::ContentType content_type = webshell::TEXT_HTML);
    virtual ~HttpException() throw();

  private:
    webshell::StatusCode _statusCode;
    mutable std::string _reasonDetail;
    webshell::ContentType _content_type;
};

} // namespace utils
