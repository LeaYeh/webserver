#pragma once

#include "defines.hpp"
#include "utils.hpp"
#include <fstream>
#include <map>

namespace webconfig
{

std::string extract_directive_value(const std::string& line,
                                    const std::string& directive);
bool revert_one_line(std::ifstream& file_stream);

webshell::ContentType get_content_type(const std::string& value);

webshell::StatusCode convert_to_status_code(const std::string& value);

} // namespace webconfig
