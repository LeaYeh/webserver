#pragma once
#include "Config.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
#include <map>
#include <string>
#include <sys/epoll.h>

namespace webkernel
{

class ConnectionHandler;

class RequestProcessor
{
  public:
    RequestProcessor(ConnectionHandler* handler);
    RequestProcessor(const RequestProcessor& other);
    RequestProcessor& operator=(const RequestProcessor& other);
    ~RequestProcessor();

    void analyze(int fd, std::string& buffer);
    void analyzeFinalize(int fd);
    void removeAnalyzer(int fd);

  private:
    RequestProcessor();
    ConnectionHandler* _handler;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;
    webshell::ResponseBuilder _builder;

    void _processGet(const webshell::Request& request);
    void _processPost(const webshell::Request& request);
    void _processPut(const webshell::Request& request);
    void _processDelete(const webshell::Request& request);
};
} // namespace webkernel
