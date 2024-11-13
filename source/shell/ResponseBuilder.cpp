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
    response.setHeader("Content-Type", "text/plain");
    response.setBody(message);

    return (response);
}

Response ResponseBuilder::buildResponse(
    StatusCode status_code, const std::map<std::string, std::string>& headers,
    const std::string& body)
{
    Response response;

    response.setStatusCode(status_code);
    response.setHeaders(headers);
    response.setBody(body);

    return (response);
}

} // namespace webshell
