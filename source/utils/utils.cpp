#include "utils.hpp"
#include <cstring>

namespace utils
{

const std::string EMPTY_STRING = "";

bool safe_close(int& fd)
{
    if (fd != -1) {
        if (close(fd) == -1) {
            return (false);
        }
        fd = -1;
    }
    return (true);
}

bool is_directory(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1) {
        return (false);
    }
    return (S_ISDIR(file_stat.st_mode));
}

std::string basefolder(const std::string& path)
{
    std::string::size_type pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return (path);
    }
    return (path.substr(0, pos));
}

std::string basename(const std::string& path)
{
    std::string::size_type pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return (path);
    }
    return (path.substr(pos + 1));
}

bool is_file(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1) {
        return (false);
    }
    return (S_ISREG(file_stat.st_mode));
}

bool start_with(const std::string& str, const std::string& prefix)
{
    return (str.find(prefix) == 0);
}

std::string file_extension(const std::string& path)
{
    std::string::size_type pos = path.find_last_of('.');
    if (pos == std::string::npos) {
        return (EMPTY_STRING);
    }
    return (path.substr(pos + 1));
}

std::string join(const std::string& path1, const std::string& path2)
{
    if (path1.empty()) {
        return (path2);
    }
    if (path2.empty()) {
        return (path1);
    }
    if (path1[path1.size() - 1] == '/' && path2[0] == '/') {
        return (path1 + path2.substr(1));
    }
    if (path1[path1.size() - 1] != '/' && path2[0] != '/') {
        return (path1 + "/" + path2);
    }
    return (path1 + path2);
}

std::string resolve_path(const std::string& path)
{
    std::vector<std::string> segments = split_path(path);
    return normalize_path_segments(segments, path[0] == '/');
}

std::vector<std::string> split_path(const std::string& path)
{
    std::vector<std::string> segments;
    size_t start = 0;

    while (true) {
        size_t pos = path.find('/', start);
        if (pos == std::string::npos) {
            std::string segment = path.substr(start);
            if (!segment.empty()) {
                segments.push_back(segment);
            }
            break;
        }
        else {
            if (pos > start) {
                segments.push_back(path.substr(start, pos - start));
            }
            start = pos + 1;
        }
    }
    return segments;
}

std::string normalize_path_segments(const std::vector<std::string>& segments,
                                    bool is_absolute)
{
    std::vector<std::string> stack;

    for (size_t i = 0; i < segments.size(); ++i) {
        const std::string& token = segments[i];

        if (token.empty() || token == ".") {
            continue;
        }
        if (token == "..") {
            if (!stack.empty()) {
                stack.pop_back();
            }
            else {
                if (!is_absolute) {
                    stack.push_back("..");
                }
            }
        }
        else {
            stack.push_back(token);
        }
    }
    std::string result;
    for (size_t i = 0; i < stack.size(); ++i) {
        if (i > 0 || is_absolute) {
            result += "/";
        }
        result += stack[i];
    }
    if (result.empty() && is_absolute) {
        result = "/";
    }
    return result;
}

// bool setup_nonblocking(int fd)
// {
//     int flags = fcntl(fd, F_GETFL, 0);

//     if (flags == -1)
//     {
//         std::cerr << "Error getting file descriptor flags: " <<
//         strerror(errno)
//                   << std::endl;
//         return (false);
//     }
//     if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
//     {
//         std::cerr << "Error setting file descriptor to non-blocking: "
//                   << strerror(errno) << std::endl;
//         return (false);
//     }
//     return (true);
// }

std::string trim(const std::string& str)
{
    std::string::size_type start = 0;
    while (start < str.size() && std::isspace(str[start])) {
        ++start;
    }
    std::string::size_type end = str.size();
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    return (str.substr(start, end - start));
}

int stoi(const std::string& str)
{
    std::istringstream iss(str);
    int result;

    iss >> result;
    if (iss.fail()) {
        throw std::invalid_argument("stoi: invalid argument: " + str);
    }
    return (result);
}

size_t convert_to_size(const std::string& str)
{
    std::string::size_type pos = str.find_last_not_of("0123456789");
    std::string size_str = str.substr(0, pos + 1);
    size_t size = stoi(size_str);

    if (pos > 3) {
        throw std::invalid_argument("convert_to_size: invalid argument: "
                                    + str);
    }
    if (str[pos] == 'b' || str[pos] == 'B') {
        size *= 1;
    }
    else if (str[pos] == 'k' || str[pos] == 'K') {
        size *= 1024;
    }
    else if (str[pos] == 'm' || str[pos] == 'M') {
        size *= 1024 * 1024;
    }
    else if (str[pos] == 'g' || str[pos] == 'G') {
        size *= 1024 * 1024 * 1024;
    }
    else {
        throw std::invalid_argument("convert_to_size: invalid argument: "
                                    + str);
    }
    return (size);
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return (tokens);
}

} // namespace utils
