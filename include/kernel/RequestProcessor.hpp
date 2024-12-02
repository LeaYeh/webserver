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
    bool setupRequestConfig(int fd, const webshell::Request& request);
    const EventProcessingState& state(int fd) const;
    void setState(int fd, EventProcessingState state);
    void resetState(int fd);

  private:
    RequestProcessor();

    ConnectionHandler* _handler;
    Reactor* _reactor;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;
    std::map<int /* fd */, EventProcessingState> _state;
    std::map<int /* fd */, webshell::Request> _request_records;
    std::map<int /* fd */, webconfig::RequestConfig> _request_config_pool;
};
} // namespace webkernel
