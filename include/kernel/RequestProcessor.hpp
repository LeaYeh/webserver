#pragma once
#include "Reactor.hpp"
#include "RequestAnalyzer.hpp"
#include "RequestConfig.hpp"
#include "Timer.hpp"
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
    void remove_analyzer(int fd);

    EventProcessingState state(int fd) const;
    void set_state(int fd, EventProcessingState state);
    void reset_state(int fd);
    void remove_state(int fd);
    bool need_to_close(int fd);

public:
    RequestProcessor(ConnectionHandler* handler);
    RequestProcessor(const RequestProcessor& other);
    RequestProcessor& operator=(const RequestProcessor& other);
    ~RequestProcessor();

private:
    ConnectionHandler* _handler;
    std::map<int /* fd */, webshell::RequestAnalyzer> _analyzer_pool;
    std::map<int /* fd */, EventProcessingState> _state;
    std::map<int /* fd */, utils::Timer> _timer_pool;
    std::map<int /* fd */, bool /* need to close */> _connection_status_pool;

private:
    void _handle_virtual_host(int fd);
    void _setup_timer(int fd, const webconfig::RequestConfig& config);
    void _handle_keep_alive(int fd);
    void _end_request(int fd);

private:
    RequestProcessor();
};
} // namespace webkernel
