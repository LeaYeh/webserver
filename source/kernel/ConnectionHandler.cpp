#include "ConnectionHandler.hpp"
#include "utils/utils.hpp"

namespace webkernel
{

ConnectionHandler::ConnectionHandler()
{
}

ConnectionHandler::ConnectionHandler(int conn_fd, Reactor* reactor)
    : _conn_fd(conn_fd), _reactor(reactor)
{
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler&)
{
}

ConnectionHandler& ConnectionHandler::operator=(const ConnectionHandler&)
{
    return (*this);
}

ConnectionHandler::~ConnectionHandler()
{
    utils::safe_close(_conn_fd);
}

void ConnectionHandler::handle_event(int fd, uint32_t events)
{
    if (events & EPOLLIN)
    {
        char buffer[1024];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read == -1)
            throw std::runtime_error("read() failed: " +
                                     std::string(strerror(errno)));
        else if (bytes_read == 0)
        {
            utils::safe_close(fd);
            _reactor->remove_handler(fd);
        }
        else
        {
            buffer[bytes_read] = '\0';
            std::cout << buffer << std::endl;
        }
    }
    else if (events & EPOLLOUT)
    {}
}


} // namespace webkernel
