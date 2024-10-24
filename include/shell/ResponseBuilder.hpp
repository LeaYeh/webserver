#pragma once

#include "Response.hpp"
#include "defines.hpp"

#include <string>

namespace webshell
{
class ResponseBuilder
{
  public:
    ResponseBuilder();
    ResponseBuilder(const ResponseBuilder& other);
    ResponseBuilder& operator=(const ResponseBuilder& other);
    ~ResponseBuilder();

    Response buildErrorResponse(StatusCode status_code, const std::string& description);

  private:
};
} // namespace webshell
