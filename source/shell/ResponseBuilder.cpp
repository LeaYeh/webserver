#include "ResponseBuilder.hpp"

namespace webshell
{

ResponseBuilder::~ResponseBuilder()
{
}

Response ResponseBuilder::buildErrorResponse(StatusCode status_code,
                                             const std::string& message)
{
    Response response;

    response.setStatusCode(status_code);
    response.clearHeaders();
    response.setHeader("Content-Type", "text/plain");
    response.setBody(message);

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
