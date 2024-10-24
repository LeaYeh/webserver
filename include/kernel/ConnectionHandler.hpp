#pragma once

#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "IHandler.hpp"
#include "Reactor.hpp"
#include "RequestProcessor.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include <ctime>
#include <errno.h>
#include <iostream>
#include <map>
#include <string.h>
#include <string>
#include <vector>

namespace webkernel
{

class ConnectionHandler : public IHandler
{
  public:
    ConnectionHandler(Reactor* reactor);
    ConnectionHandler(const ConnectionHandler& other);
    ConnectionHandler& operator=(const ConnectionHandler& other);
    ~ConnectionHandler();

    void handleEvent(int fd, uint32_t events);
    void closeConnection(int fd, weblog::LogLevel level, std::string message);
    void prepareWrite(int fd, const std::string& buffer);

  private:
    Reactor* _reactor;
    RequestProcessor _processor;
    // TODO: Use _read_buffer to handle extra data which out of a request
    // TODO: Consider to use pair instead of map
    std::map<int /*fd*/, std::string> _read_buffer;
    std::map<int /*fd*/, std::string> _write_buffer;

    ConnectionHandler();
    void _handleRead(int fd);
    void _handleWrite(int fd);

    // bool _keepAlive(void);
};

} // namespace webkernel
