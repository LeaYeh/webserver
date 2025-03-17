#include "ResponseBuilder.hpp"
#include "Config.hpp"
#include "TemplateEngine.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <string>

namespace webshell
{

ResponseBuilder::~ResponseBuilder() {}

Response ResponseBuilder::ok(StatusCode status_code,
                             const std::map<std::string, std::string>& headers,
                             const std::string& body,
                             bool body_only)
{
    Response response;

    if (body_only) {
        response.set_status_code(IGNORE);
        response.clear_headers();
        response.set_body(body);
    }
    else {
        response.set_status_code(status_code);
        response.set_headers(headers);
        response.set_body(body);
    }

    return (response);
}

Response ResponseBuilder::redirect(StatusCode status_code,
                                   const std::string& location)
{
    Response response;

    response.set_status_code(status_code);
    response.clear_headers();
    response.set_header("Content-Type", content_type_to_string(TEXT_PLAIN));
    response.set_header("Location", location);

    return (response);
}

Response ResponseBuilder::error(StatusCode status_code,
                                const std::string& description,
                                const ContentType& content_type)
{
    Response response;

    response.set_status_code(status_code);
    response.clear_headers();
    response.set_header("Content-Type", content_type_to_string(content_type));

    switch (content_type) {
    case TEXT_HTML:
        response.set_body(_render_error_page(status_code, description));
        break;
    case APPLICATION_JSON:
        response.set_body(_render_json(status_code, description));
        break;
    default:
        response.set_body(description);
        break;
    }

    return (response);
}

std::string ResponseBuilder::_render_error_page(StatusCode status_code,
                                                const std::string& message)
{
    static webkernel::TemplateEngine template_engine;

    try {
        template_engine.load_template(
            webconfig::Config::instance()->http_block().error_page());
        template_engine.set_variable("STATUS_CODE",
                                     utils::to_string(status_code));
        template_engine.set_variable("ERROR_REASON", message);

        return (template_engine.render());
    }
    catch (const std::exception& e) {
        // return (error(INTERNAL_SERVER_ERROR, e.what(), TEXT_PLAIN).body());
        return ("<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "<title>Error</title>\n"
                "</head>\n"
                "<body>\n"
                "<h1>Error "
                + utils::to_string(status_code)
                + "</h1>\n"
                  "<p>"
                + message
                + "</p>\n"
                  "</body>\n"
                  "</html>\n");
    }
}

std::string ResponseBuilder::_render_json(StatusCode status_code,
                                          const std::string& message)
{
    return ("{\"status_code\": " + utils::to_string(status_code)
            + ", \"message\": \"" + message + "\"}");
}

} // namespace webshell
