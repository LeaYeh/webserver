#include "defines.hpp"
#include <string>

namespace webshell
{

std::string statusReasonPhase(const StatusCode& statusCode);
std::string requestMethodToString(webshell::RequestMethod method);
std::string contentTypeToString(webshell::ContentType type);

} // namespace webshell
