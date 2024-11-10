#pragma once
#include "Reactor.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
#include "RequestConfig.hpp"
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

    bool analyze(int fd, std::string& buffer);
    void process(int fd);
    void removeAnalyzer(int fd);

    const webconfig::RequestConfig& requestConfig(int fd) const;
    void setupRequestConfig(int fd, const webshell::Request& request);

  private:
    RequestProcessor();

    ConnectionHandler* _handler;
    const Reactor* _reactor;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;
    webconfig::RequestConfig _request_config;
    // void _processGet(int fd, const webshell::Request& request);
    // void _processPost(int fd, const webshell::Request& request);
    // void _processPut(int fd, const webshell::Request& request);
    // void _processDelete(int fd, const webshell::Request& request);
};
} // namespace webkernel
