#pragma once

#include "defines.hpp"
#include <algorithm>
#include <cctype> // for std::isspace
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

namespace utils
{

bool safe_close(int& fd);
bool safe_close_pipe(int (&pipe_fd)[2]);
bool setup_nonblocking(int fd);
std::string trim(const std::string& str);
int stoi(const std::string& str);
unsigned int convert_to_size(const std::string& str);
std::vector<std::string> split(const std::string& str, char delim);
std::string request_method_to_string(webshell::RequestMethod method);
std::string content_type_to_string(webshell::ContentType type);

template <typename T> std::string to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

} // namespace utils
