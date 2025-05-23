#include "Acceptor.hpp"
#include "ConnectionHandler.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <sys/epoll.h>

namespace webkernel
{

Acceptor::Acceptor()
{
    LOG(weblog::DEBUG, "Acceptor created");
}

Acceptor::~Acceptor()
{
    LOG(weblog::DEBUG, "Acceptor destroyed");
}

// The ConnectionHandler object is created and registered with the Reactor
// the object will be deleted by the Reactor when the connection is closed
// Register the connection handler with the reactor and set it to epoll edge
// triggered mode with Level Triggered (LT) mode as the default
void Acceptor::handle_event(int fd, uint32_t events)
{
    if (events & EPOLLIN) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int conn_fd = accept(fd, (struct sockaddr*)&client_addr, &addr_size);

        LOG(weblog::DEBUG,
            "Accepted connection on fd[" + utils::to_string(conn_fd)
                + "] from: " + get_client_address(conn_fd));
        if (conn_fd < 0) {
            throw std::runtime_error("accept() failed");
        }
        Reactor::instance()->register_handler(conn_fd,
                                              ConnectionHandler::instance(),
                                              EPOLLIN | EPOLLHUP | EPOLLERR);
        LOG(weblog::DEBUG,
            "Registered connection handler with fd: "
                + utils::to_string(conn_fd));
    }
    else {
        LOG(weblog::ERROR,
            "Acceptor got unknown event: " + explain_epoll_event(events));
    }
}

} // namespace webkernel
