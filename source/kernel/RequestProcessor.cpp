#include "RequestProcessor.hpp"
#include "ConnectionHandler.hpp"
#include "Logger.hpp"
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

RequestProcessor::RequestProcessor(ConnectionHandler* handler)
    : _handler(handler), _analyzer_pool()
{
    _reactor = handler->reactor();
}

RequestProcessor::RequestProcessor(const RequestProcessor& other)
    : _handler(other._handler), _reactor(other._reactor),
      _analyzer_pool(other._analyzer_pool)
{
}

RequestProcessor& RequestProcessor::operator=(const RequestProcessor& other)
{
    if (this != &other)
    {
        _handler = other._handler;
        _analyzer_pool = other._analyzer_pool;
        _handler = other._handler;
        _reactor = other._reactor;
    }
    return (*this);
}

RequestProcessor::~RequestProcessor()
{
}

// Analyze the buffer and feed to the request analyzer char by char to avoid
// lossing data which belongs to the next request
// TODO: How to test this function?
// TODO: If there are multiple requests in the buffer, we need to handle
// them????? HOW?
bool RequestProcessor::analyze(int fd, std::string& buffer)
{
    size_t i = 0;

    if (_analyzer_pool.find(fd) == _analyzer_pool.end())
    {
        _analyzer_pool[fd] = webshell::RequestAnalyzer(
            _reactor->lookupServerId(fd),
            &buffer); // TODO: import read buffer and CONST request config ref
                      // body limit
        resetState(fd);
    }

    while (i < buffer.size())
    {
        _analyzer_pool[fd].feed(buffer[i]);
        if (_analyzer_pool[fd].isComplete() /* && (i < buffer.size() - 1)*/)
        {
            weblog::Logger::log(weblog::WARNING,
                                "Request is complete: \n" +
                                    _analyzer_pool[fd].request().serialize());
            buffer.erase(0, i + 1);
            process(fd);
            return (true);
        }
        i++;
    }
    buffer.erase(0, i);
    return (_analyzer_pool[fd].isComplete());
}

// This function can be called after the request is complete or the request need
// to be processed in chunks
void RequestProcessor::process(int fd)
{
    RequestHandlerManager* manager = &RequestHandlerManager::getInstance();
    EventProcessingState& state = _state[fd];
    webshell::Request request = _analyzer_pool[fd].request();
    webshell::Response response = manager->handleRequest(fd, state, request);

    if (request.method() == webshell::POST && (state & HANDLE_CHUNKED))
        _reactor->modifyHandler(fd, 0, EPOLLIN);
    if (request.method() == webshell::POST && request.empty_buffer())
        _reactor->modifyHandler(fd, EPOLLIN, 0);
    weblog::Logger::log(weblog::DEBUG,
                        "state: " + explainEventProcessingState(state));

    if (request.method() == webshell::POST && (state & COMPELETED))
        _handler->prepareWrite(fd, response.serialize());
    else if (request.method() == webshell::GET)
        _handler->prepareWrite(fd, response.serialize());

    // TODO: After processing the request, we need to reset the analyzer or when
    // it is times out we need to remove it
    if (request.method() == webshell::POST && (state & COMPELETED))
        _analyzer_pool[fd].reset();
    else if (request.method() == webshell::GET)
        _analyzer_pool[fd].reset();
}

void RequestProcessor::removeAnalyzer(int fd)
{
    _analyzer_pool.erase(fd);
}

EventProcessingState RequestProcessor::state(int fd) const
{
    if (_state.find(fd) == _state.end())
        return (UNKNOWN);
    return (_state.at(fd));
}

void RequestProcessor::setState(int fd, EventProcessingState state)
{
    _state[fd] = state;
}

void RequestProcessor::resetState(int fd)
{
    _state[fd] = INITIAL;
}

} // namespace webkernel
