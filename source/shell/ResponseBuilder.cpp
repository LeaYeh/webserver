#include "ResponseBuilder.hpp"
#include "utils/utils.hpp"

namespace webshell
{

ResponseBuilder::ResponseBuilder()
{
}

ResponseBuilder::ResponseBuilder(const ResponseBuilder& other)
{
    (void)other;
}

ResponseBuilder& ResponseBuilder::operator=(const ResponseBuilder& other)
{
    (void)other;
    return (*this);
}

ResponseBuilder::~ResponseBuilder()
{
}

Response ResponseBuilder::buildErrorResponse(StatusCode status_code, const std::string& message)
{
    Response response;

    response.setStatusCode(status_code);
    response.setHeader("Content-Length", utils::toString(message.size()));
    response.setHeader("Content-Type", "text/plain");
    response.setBody(message);

    return (response);
}

} // namespace webshell