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
} // namespace utils
