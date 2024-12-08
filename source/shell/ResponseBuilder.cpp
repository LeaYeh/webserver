#include "ResponseBuilder.hpp"
#include "TemplateEngine.hpp"
#include "utils.hpp"

namespace webshell
{

ResponseBuilder::~ResponseBuilder()
{
}

Response ResponseBuilder::buildErrorResponse(StatusCode status_code,
                                             const std::string& message)
{
    static webkernel::TemplateEngine template_engine;
    Response response;

    response.setStatusCode(status_code);
    response.clearHeaders();
    response.setHeader("Content-Type", "text/html");

    template_engine.loadTemplate("./www/html/error_page.html");
    template_engine.setVariable("STATUS_CODE", utils::toString(status_code));
    template_engine.setVariable("ERROR_REASON", message);
    response.setBody(template_engine.render());

    return (response);
}

Response ResponseBuilder::buildResponse(
    StatusCode status_code, const std::map<std::string, std::string>& headers,
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

} // namespace webshell
