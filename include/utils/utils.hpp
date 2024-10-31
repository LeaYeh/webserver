#pragma once

#include "defines.hpp"
#include <cctype> // for std::isspace
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace utils
{

bool safeClose(int& fd);
// bool setup_nonblocking(int fd);
std::string trim(const std::string& str);
int stoi(const std::string& str);
unsigned int convertToSize(const std::string& str);
std::vector<std::string> split(const std::string& str, char delim);
bool isDirectory(const std::string& path);
bool isFile(const std::string& path);
std::string EMPTY_STRING = "";

template <typename T> std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

} // namespace utils
