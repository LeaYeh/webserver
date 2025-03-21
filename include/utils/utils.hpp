#pragma once

#include <cctype> // for std::isspace
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace utils
{

extern const std::string EMPTY_STRING;
bool safe_close(int& fd);
// bool setup_nonblocking(int fd);
std::string trim(const std::string& str);
int stoi(const std::string& str);
size_t convert_to_size(const std::string& str);
std::vector<std::string> split(const std::string& str, char delim);
bool is_directory(const std::string& path);
bool is_file(const std::string& path);
// bool is_tchar(unsigned char c);
bool start_with(const std::string& str, const std::string& prefix);
std::string basefolder(const std::string& path);
std::string basename(const std::string& path);
std::string file_extension(const std::string& path);
std::string join(const std::string& path1, const std::string& path2);
std::vector<std::string> split_path(const std::string& path);
std::string normalize_path_segments(const std::vector<std::string>& segments,
                                    bool is_absolute);
std::string resolve_path(const std::string& path);

template <typename T> std::string to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

} // namespace utils
