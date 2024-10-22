#pragma once
#include "Config.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
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

    void analyze(const char* buffer, size_t size);
    bool isRequestComplete();
    void analyzeFinalize(int fd);

  private:
    RequestProcessor();
    ConnectionHandler* _handler;
    webshell::RequestAnalyzer _analyzer;
    webshell::ResponseBuilder _builder;

    void _processGet(const webshell::Request& request);
    void _processPost(const webshell::Request& request);
    void _processPut(const webshell::Request& request);
    void _processDelete(const webshell::Request& request);
};
} // namespace webkernel
