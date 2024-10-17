#pragma once

#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "HttpRequestParser.hpp"
#include "IHandler.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include <ctime>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <string>

namespace webkernel
{

class ConnectionHandler : public IHandler
{
  public:
    ConnectionHandler();
    ConnectionHandler(int conn_fd, Reactor* reactor, webconfig::Config* config,
                      int server_id);
    ConnectionHandler(const ConnectionHandler& other);
    ConnectionHandler& operator=(const ConnectionHandler& other);
    ~ConnectionHandler();

    void handleEvent(int fd, uint32_t events);

  private:
    int _conn_fd;
    time_t _start_time;
    Reactor* _reactor;
    int _server_id;
    webconfig::ConfigHttpBlock _http_config;
    webconfig::ConfigServerBlock _server_config;
    webshell::HttpRequestParser _request_parser;

    void _setupConfig(void);

    void _handleRead(int fd);
    void _handleWrite(int fd);
    void _handleClose(int fd, weblog::LogLevel level, std::string message);

    bool _keepAlive(void);
    void _processRequest(void);
    void _respond(int fd);
};

} // namespace webkernel
