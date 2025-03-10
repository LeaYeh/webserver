
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
    webshell::StatusCode status_code() const;
    std::string reasonDetail() const;
    webshell::ContentType contentType() const;

public:
    HttpException(const webshell::StatusCode& status_code,
                  const std::string& reasonDetail,
                  webshell::ContentType content_type = webshell::TEXT_HTML);
    HttpException(const HttpException& other);
    HttpException& operator=(const HttpException& other);
    virtual ~HttpException() throw();

protected:
    webshell::StatusCode _statusCode;
    mutable std::string _reasonDetail;
    webshell::ContentType _content_type;
};

} // namespace utils
