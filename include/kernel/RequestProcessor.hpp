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
    bool analyze(int fd, std::string& buffer);
    void process(int fd);
    void removeAnalyzer(int fd);

    EventProcessingState state(int fd) const;
    void setState(int fd, EventProcessingState state);
    void resetState(int fd);

public:
    RequestProcessor(ConnectionHandler* handler);
    RequestProcessor(const RequestProcessor& other);
    RequestProcessor& operator=(const RequestProcessor& other);
    ~RequestProcessor();

private:
    ConnectionHandler* _handler;
    Reactor* _reactor;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;
    std::map<int /* fd */, EventProcessingState> _state;

private:
    void _handle_virtual_host(int fd);

private:
    RequestProcessor();
};
} // namespace webkernel
