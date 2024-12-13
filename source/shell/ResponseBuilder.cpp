#include "ResponseBuilder.hpp"
#include "TemplateEngine.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <string>

namespace webshell
{

ResponseBuilder::~ResponseBuilder()
{
}

Response ResponseBuilder::ok(StatusCode status_code,
                             const std::map<std::string, std::string>& headers,
                             const std::string& body, bool body_only)
{
    Response response;

    if (body_only)
    {
        response.setStatusCode(UNDEFINED);
        response.clearHeaders();
        response.setBody(body);
    }
    else
    {
        response.setStatusCode(status_code);
        response.setHeaders(headers);
        response.setBody(body);
    }

    return (response);
}

Response ResponseBuilder::redirect(StatusCode status_code,
                                   const std::string& location)
{
    Response response;

    response.setStatusCode(status_code);
    response.clearHeaders();
    response.setHeader("Content-Type", contentTypeToString(TEXT_PLAIN));
    response.setHeader("Location", location);

    return (response);
}

Response ResponseBuilder::error(StatusCode status_code,
                                const std::string& description,
                                const ContentType& content_type)
{
    Response response;

    response.setStatusCode(status_code);
    response.clearHeaders();
    response.setHeader("Content-Type", contentTypeToString(content_type));

    switch (content_type)
    {
    case TEXT_HTML:
        response.setBody(_render_error_page(status_code, description));
        break;
    case APPLICATION_JSON:
        response.setBody(_render_json(status_code, description));
        break;
    default:
        response.setBody(description);
        break;
    }

    return (response);
}

std::string ResponseBuilder::_render_error_page(StatusCode status_code,
                                                const std::string& message)
{
    static webkernel::TemplateEngine template_engine;

    // TODO: Replace the error page path with config
    template_engine.loadTemplate("./www/html/error_page.html");
    template_engine.setVariable("STATUS_CODE", utils::toString(status_code));
    template_engine.setVariable("ERROR_REASON", message);

    return (template_engine.render());
}

std::string ResponseBuilder::_render_json(StatusCode status_code,
                                          const std::string& message)
{
    return ("{\"status_code\": " + utils::toString(status_code) +
            ", \"message\": \"" + message + "\"}");
}

} // namespace webshell
