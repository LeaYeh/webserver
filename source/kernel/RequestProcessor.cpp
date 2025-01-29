#include "RequestProcessor.hpp"
#include "ConnectionHandler.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
#include "RequestHandlerManager.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <string>
#include <sys/epoll.h>

namespace webkernel
{

RequestProcessor::RequestProcessor(ConnectionHandler* handler) :
    _handler(handler), _analyzer_pool()
{
}

RequestProcessor::RequestProcessor(const RequestProcessor& other) :
    _handler(other._handler),
    _analyzer_pool(other._analyzer_pool)
{
}

RequestProcessor& RequestProcessor::operator=(const RequestProcessor& other)
{
    if (this != &other) {
        _handler = other._handler;
        _analyzer_pool = other._analyzer_pool;
        _handler = other._handler;
    }
    return (*this);
}

RequestProcessor::~RequestProcessor() {}

// Analyze the buffer and feed to the request analyzer char by char to avoid
// lossing data which belongs to the next request
bool RequestProcessor::analyze(int fd, std::string& buffer)
{
    size_t i = 0;

    if (_analyzer_pool.find(fd) == _analyzer_pool.end()) {
        _analyzer_pool[fd] = webshell::RequestAnalyzer(&buffer);
        reset_state(fd);
    }
    while (i < buffer.size()) {
        _analyzer_pool[fd].feed(buffer[i]);
        if (_analyzer_pool[fd].is_complete()) {
            _handle_virtual_host(fd);
            _setup_timer(fd, _analyzer_pool[fd].request().config());
            buffer.erase(0, i + 1);
            process(fd);
            return (true);
        }
        i++;
    }
    buffer.erase(0, i);
    return (_analyzer_pool[fd].is_complete());
}

void RequestProcessor::_setup_timer(int fd,
                                    const webconfig::RequestConfig& config)
{
    if (_timer_pool.find(fd) == _timer_pool.end()) {
        _timer_pool[fd] = utils::Timer(config.keep_alive_timeout);
    }
    _timer_pool[fd].start();
}

// This function can be called after the request is complete or the request need
// to be processed in chunks
void RequestProcessor::process(int fd)
{
    RequestHandlerManager* manager = &RequestHandlerManager::get_instance();
    EventProcessingState& state = _state[fd];
    webshell::Request& request = _analyzer_pool[fd].request();
    webshell::Response response = manager->handle_request(fd, state, request);

    weblog::Logger::log(weblog::DEBUG,
                        "state: " + explain_event_processing_state(state));

    if (request.method() == webshell::POST) {
        // if the server still processing the upload data, we need to consume
        // the read buffer first and stop reading new requests
        if (state & HANDLE_CHUNKED) {
            Reactor::instance()->modify_handler(fd, EPOLLOUT, EPOLLIN);
        }
        // if the read buffer is empty but the server still uploading data, we
        // need to wait for more data
        if (request.empty_buffer()) {
            Reactor::instance()->modify_handler(fd, EPOLLIN, 0);
        }
        if (state & COMPELETED) {
            _handler->prepare_write(fd, response.serialize());
            _end_request(fd);
        }
    }
    // for GET and DELETE requests, we can send the response directly
    else {
        _handler->prepare_write(fd, response.serialize());
        if (state & COMPELETED) {
            _end_request(fd);
        }
    }
}

void RequestProcessor::remove_analyzer(int fd)
{
    _analyzer_pool.erase(fd);
}

EventProcessingState RequestProcessor::state(int fd) const
{
    if (_state.find(fd) == _state.end()) {
        return (UNKNOWN);
    }
    return (_state.at(fd));
}

void RequestProcessor::set_state(int fd, EventProcessingState state)
{
    _state[fd] = state;
}

void RequestProcessor::reset_state(int fd)
{
    _state[fd] = INITIAL;
}

void RequestProcessor::_handle_keep_alive(int fd)
{
    const webshell::Request& request = _analyzer_pool[fd].request();

    if (!request.has_header("connection")
        || request.get_header("connection") == "close") {
        _handler->close_connection(fd, weblog::INFO, "Connection: close");
        _timer_pool.erase(fd);
    }
    else if (_timer_pool[fd].timeout()) {
        _handler->close_connection(
            fd,
            weblog::INFO,
            "Keep-alive timeout: " + utils::to_string(_timer_pool[fd].elapsed())
                + " seconds, close the connection");
        _timer_pool.erase(fd);
    }
    else {
        weblog::Logger::log(weblog::DEBUG,
                            "Keep-alive connection, time passed: "
                                + utils::to_string(_timer_pool[fd].elapsed())
                                + " seconds");
    }
}

void RequestProcessor::_end_request(int fd)
{
    _handle_keep_alive(fd);
    _analyzer_pool.erase(fd);
}

void RequestProcessor::_handle_virtual_host(int fd)
{
    VirtualHostManager& vhost_manager = _handler->vhost_manager;
    const std::string& host = _analyzer_pool[fd].request().get_header("host");
    webshell::Request& request = _analyzer_pool[fd].request();
    webconfig::ConfigServerBlock* default_server =
        vhost_manager.find_default(fd);
    const std::string& ipaddr = get_socket_address(fd);
    webconfig::ConfigServerBlock* server_config = NULL;

    if (request.uri().type == webshell::ORIGIN) {
        server_config = default_server;
        if (!host.empty()) {
            server_config = vhost_manager.find_server(ipaddr, host);
            if (server_config == NULL) {
                throw utils::HttpException(webshell::NOT_FOUND,
                                           "No virtual host found: " + host);
            }
        }
    }
    else if (request.uri().type == webshell::ABSOLUTE) {
        server_config =
            vhost_manager.find_server(ipaddr, request.uri().authority);
        if (server_config == NULL) {
            server_config = default_server;
        }
    }
    else {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Invalid request URI");
    }
    request.setup_config(server_config);
}

} // namespace webkernel
