#pragma once

#include "Response.hpp"
#include "defines.hpp"

#include <string>

namespace webshell
{
class ResponseBuilder
{
  public:
    ~ResponseBuilder();
    static Response buildErrorResponse(StatusCode status_code,
                                       const std::string& description);
    static Response
    buildResponse(StatusCode status_code,
                  const std::map<std::string, std::string>& headers,
                  const std::string& body);

  private:
    ResponseBuilder();
    ResponseBuilder(const ResponseBuilder& other);
    ResponseBuilder& operator=(const ResponseBuilder& other);
};
} // namespace webshell
