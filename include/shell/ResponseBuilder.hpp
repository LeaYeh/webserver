#pragma once

#include "Response.hpp"
#include "defines.hpp"
#include <string>

namespace webshell
{
class ResponseBuilder
{
  public:
    static Response buildErrorResponse(
        StatusCode status_code, const std::string& description,
        webshell::ContentType content_type = webshell::TEXT_HTML);
    static Response
    buildResponse(StatusCode status_code,
                  const std::map<std::string, std::string>& headers,
                  const std::string& body, bool body_only);

  public:
    ~ResponseBuilder();

  private:
    ResponseBuilder();
    ResponseBuilder(const ResponseBuilder& other);
    ResponseBuilder& operator=(const ResponseBuilder& other);
};
} // namespace webshell
