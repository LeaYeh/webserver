#pragma once

#include "HttpException.hpp"
#include "IHandler.hpp"
#include "Reactor.hpp"
#include "RequestProcessor.hpp"
#include "defines.hpp"
#include <ctime>
#include <map>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

namespace webkernel
{

static const size_t BUFFER_SIZE = 4096;
class ConnectionHandler : public IHandler
{
  public:

    ConnectionHandler(Reactor* reactor);
    ~ConnectionHandler();

    Reactor* reactor(void) const;

    void handleEvent(int fd, uint32_t events);
    void closeConnection(int fd, weblog::LogLevel level, std::string message);
    void prepareWrite(int fd, const std::string& buffer);
    void prepareError(int fd, const utils::HttpException& e);

  private:
    Reactor* _reactor;
    RequestProcessor _processor;
    // TODO: Use _read_buffer to handle extra data which out of a request
    // TODO: Consider to use pair instead of map
    std::map<int /*fd*/, std::string> _read_buffer;
    std::map<int /*fd*/, std::string> _write_buffer;
    std::map<int /*fd*/, std::string> _error_buffer;

  private:
    void _handleRead(int fd);
    void _handleWrite(int fd);
    void _sendNormal(int fd);
    void _sendError(int fd);
    bool _is_buffer_full(const std::string& buffer) const;

  private:
    ConnectionHandler();
    ConnectionHandler(const ConnectionHandler& other);
    ConnectionHandler& operator=(const ConnectionHandler& other);
    // bool _keepAlive(void);
};

} // namespace webkernel
