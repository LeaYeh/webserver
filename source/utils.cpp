#include "utils.hpp"

namespace utils
{
bool safe_close(int& fd)
{
    if (fd != -1)
    {
        if (close(fd) == -1)
            return (false);
        fd = -1;
    }
    return (true);
}

bool safe_close_pipe(int (&pipe_fd)[2])
{
    if (!safe_close(pipe_fd[0]) || !safe_close(pipe_fd[1]))
        return (false);
    return (true);
}
bool setup_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
    {
        std::cerr << "Error getting file descriptor flags: " << strerror(errno)
                  << std::endl;
        return (false);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error setting file descriptor to non-blocking: "
                  << strerror(errno) << std::endl;
        return (false);
    }
    return (true);
}

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

unsigned int convert_to_size(const std::string& str)
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

} // namespace utils
