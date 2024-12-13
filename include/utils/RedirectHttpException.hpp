#pragma once
#include "HttpException.hpp"
#include "defines.hpp"

namespace utils
{

class RedirectHttpException : public HttpException
{
  public:
    RedirectHttpException(const webshell::StatusCode& statusCode,
                          const std::string& reasonDetail);
    virtual ~RedirectHttpException() throw();

  private:
    RedirectHttpException();
    RedirectHttpException(const RedirectHttpException& other);
    RedirectHttpException& operator=(const RedirectHttpException& other);
};

} // namespace utils
