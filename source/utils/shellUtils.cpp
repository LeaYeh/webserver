#include "shellUtils.hpp"
#include "defines.hpp"

namespace webshell
{

std::string status_reason_phase(const StatusCode& status_code)
{
    switch (status_code) {
    case OK:
        return ("OK");
    case CREATED:
        return ("Created");
    case ACCEPTED:
        return ("Accepted");
    case NO_CONTENT:
        return ("No Content");
    case MOVED_PERMANENTLY:
        return ("Moved Permanently");
    case FOUND:
        return ("Found");
    case SEE_OTHER:
        return ("See Other");
    case NOT_MODIFIED:
        return ("Not Modified");
    case BAD_REQUEST:
        return ("Bad Request");
    case UNAUTHORIZED:
        return ("Unauthorized");
    case FORBIDDEN:
        return ("Forbidden");
    case NOT_FOUND:
        return ("Not Found");
    case METHOD_NOT_ALLOWED:
        return ("Method Not Allowed");
    case EXPECTATION_FAILED:
        return ("Expectation Failed");
    case REQUEST_TIMEOUT:
        return ("Request Timeout");
    case NO_CONTENT_LENGTH:
        return ("Length Required");
    case INTERNAL_SERVER_ERROR:
        return ("Internal Server Error");
    case NOT_IMPLEMENTED:
        return ("Not Implemented");
    case BAD_GATEWAY:
        return ("Bad Gateway");
    case SERVICE_UNAVAILABLE:
        return ("Service Unavailable");
    case PAYLOAD_TOO_LARGE:
        return ("Payload Too Large");
    case CONTINUE:
        return ("Continue");
    default:
        return ("Unknown Status Code");
    }
}

std::string request_method_to_string(webshell::RequestMethod method)
{
    switch (method) {
    case webshell::GET:
        return ("GET");
    case webshell::POST:
        return ("POST");
    case webshell::DELETE:
        return ("DELETE");
    case webshell::CONNECT:
        return ("CONNECT");
    case webshell::OPTIONS:
        return ("OPTIONS");
    default:
        return ("UNKNOWN");
    }
}

std::string content_type_to_string(webshell::ContentType type)
{
    switch (type) {
    case webshell::TEXT_HTML:
        return ("text/html");
    case webshell::TEXT_CSS:
        return ("text/css");
    case webshell::TEXT_PLAIN:
        return ("text/plain");
    case webshell::IMAGE_JPEG:
        return ("image/jpeg");
    case webshell::IMAGE_PNG:
        return ("image/png");
    case webshell::IMAGE_GIF:
        return ("image/gif");
    case webshell::APPLICATION_OCTET_STREAM:
        return ("application/octet-stream");
    case webshell::APPLICATION_JSON:
        return ("application/json");
    default:
        return ("UNKNOWN");
    }
}

} // namespace webshell
