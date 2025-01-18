#include "defines.hpp"
#include <string>

namespace webshell
{

std::string status_reason_phase(const StatusCode& status_code);
std::string request_method_to_string(webshell::RequestMethod method);
std::string content_type_to_string(webshell::ContentType type);

} // namespace webshell
