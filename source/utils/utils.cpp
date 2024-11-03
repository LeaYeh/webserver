#include "utils/utils.hpp"

namespace utils
{

const std::string EMPTY_STRING = "";

bool safeClose(int& fd)
{
    if (fd != -1)
    {
        if (close(fd) == -1)
            return (false);
        fd = -1;
    }
    return (true);
}

bool isDirectory(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1)
        return (false);
    return (S_ISDIR(file_stat.st_mode));
}

bool isFile(const std::string& path)
{
    struct stat file_stat;

    if (stat(path.c_str(), &file_stat) == -1)
        return (false);
    return (S_ISREG(file_stat.st_mode));
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
    while (start < str.size() && std::isspace(str[start]))
        ++start;
    std::string::size_type end = str.size();
    while (end > start && std::isspace(str[end - 1]))
        --end;

    return (str.substr(start, end - start));
}

int stoi(const std::string& str)
{
    std::istringstream iss(str);
    int result;

    iss >> result;
    if (iss.fail())
        throw std::invalid_argument("stoi: invalid argument");
    return (result);
}

unsigned int convertToSize(const std::string& str)
{
    std::string::size_type pos = str.find_last_not_of("0123456789");
    std::string size_str = str.substr(0, pos + 1);
    unsigned int size = stoi(size_str);

    if (str[pos] == 'k' || str[pos] == 'K')
        size *= 1024;
    else if (str[pos] == 'm' || str[pos] == 'M')
        size *= 1024 * 1024;
    else if (str[pos] == 'g' || str[pos] == 'G')
        size *= 1024 * 1024 * 1024;
    return (size);
}

std::vector<std::string> split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);

    return (tokens);
}

} // namespace utils
