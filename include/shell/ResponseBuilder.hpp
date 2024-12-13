#pragma once

#include "Response.hpp"
#include "defines.hpp"
#include <fcntl.h>
#include <string>

namespace webshell
{
class ResponseBuilder
{
  public:
    static Response ok(StatusCode status_code,
                       const std::map<std::string, std::string>& headers,
                       const std::string& body, bool body_only = false);
    static Response error(StatusCode status_code,
                          const std::string& description,
                          const std::string& accept);
    static Response redirect(StatusCode status_code,
                             const std::string& location);
    // static Response buildErrorResponse(
    //     StatusCode status_code, const std::string& description,
    //     webshell::ContentType content_type = webshell::TEXT_HTML);
    // static Response
    // buildResponse(StatusCode status_code,
    //               const std::map<std::string, std::string>& headers,
    //               const std::string& body, bool body_only);
    static std::string negotiate_content_type(const std::string& accept_header,
                                              const ContentType& content_type);

  public:
    ~ResponseBuilder();

  private:
    static std::string _render_error_page(StatusCode status_code,
                                          const std::string& message);
    static std::string _render_json(StatusCode status_code,
                                    const std::string& message);

  private:
    ResponseBuilder();
    ResponseBuilder(const ResponseBuilder& other);
    ResponseBuilder& operator=(const ResponseBuilder& other);
};
} // namespace webshell
