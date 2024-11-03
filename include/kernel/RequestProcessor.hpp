#pragma once
#include "Reactor.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
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
    const Reactor* _reactor;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;

    void _processGet(int fd, const webshell::Request& request);
    void _processPost(int fd, const webshell::Request& request);
    void _processPut(int fd, const webshell::Request& request);
    void _processDelete(int fd, const webshell::Request& request);
};
} // namespace webkernel
