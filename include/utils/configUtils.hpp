#pragma once
#include "defines.hpp"
#include <fstream>

namespace webconfig
{

std::string extract_directive_value(const std::string& line,
                                    const std::string& directive);
bool revert_one_line(std::ifstream& file_stream);
webshell::ContentType string_to_content_type(const std::string& value);
webshell::StatusCode string_to_status_code(const std::string& value);
weblog::LogLevel string_to_level(const std::string& value);
webshell::RequestMethod string_to_request_method(const std::string& value);
std::string level_to_string(weblog::LogLevel level);

} // namespace webconfig
