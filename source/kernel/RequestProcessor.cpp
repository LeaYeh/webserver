#include "RequestProcessor.hpp"
#include "ConnectionHandler.hpp"
#include "Response.hpp"
#include "ResponseBuilder.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include <cstddef>

namespace webkernel
{

RequestProcessor::RequestProcessor(ConnectionHandler* handler)
    : _handler(handler), _analyzer_pool()
{
}

RequestProcessor::RequestProcessor(const RequestProcessor& other)
    : _handler(other._handler), _analyzer_pool(other._analyzer_pool)
{
}

RequestProcessor& RequestProcessor::operator=(const RequestProcessor& other)
{
    if (this != &other)
    {
        _handler = other._handler;
        _analyzer_pool = other._analyzer_pool;
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
void RequestProcessor::analyze(int fd, std::string& buffer)
{
    size_t i = 0;

    if (_analyzer_pool.find(fd) == _analyzer_pool.end())
        _analyzer_pool[fd] = webshell::RequestAnalyzer();
    while (i < buffer.size())
    {
        _analyzer_pool[fd].feed(buffer[i]);
        if (_analyzer_pool[fd].isComplete() && (i < buffer.size() - 1))
        {
            analyzeFinalize(fd);
            break;
        }
        i++;
    }
    buffer.erase(0, i);
}

void RequestProcessor::analyzeFinalize(int fd)
{
    // TODO: Handle invalid request by exception
    if (true || _analyzer_pool[fd].hasError())
    {
        weblog::Logger::log(weblog::DEBUG,
                            "Error detected in request analyzer: " +
                                _analyzer_pool[fd].statusInfo().second);
        webshell::Response resp = webshell::ResponseBuilder::buildErrorResponse(
            _analyzer_pool[fd].statusInfo().first,
            _analyzer_pool[fd].statusInfo().second);
        _handler->prepareWrite(fd, resp.serialize());
        return;
    }
    if (_analyzer_pool[fd].request().method() == webshell::GET)
        _processGet(_analyzer_pool[fd].request());
    else if (_analyzer_pool[fd].request().method() == webshell::POST)
        _processPost(_analyzer_pool[fd].request());
    else if (_analyzer_pool[fd].request().method() == webshell::PUT)
        _processPut(_analyzer_pool[fd].request());
    else if (_analyzer_pool[fd].request().method() == webshell::DELETE)
        _processDelete(_analyzer_pool[fd].request());

    // TODO: After processing the request, we need to reset the analyzer or when
    // it is times out we need to remove it
    // _analyzer_pool[fd].reset();
}

void RequestProcessor::removeAnalyzer(int fd)
{
    _analyzer_pool.erase(fd);
}

void RequestProcessor::_processGet(const webshell::Request& request)
{
    (void)request;
}

void RequestProcessor::_processPost(const webshell::Request& request)
{
    (void)request;
}

void RequestProcessor::_processPut(const webshell::Request& request)
{
    (void)request;
}

void RequestProcessor::_processDelete(const webshell::Request& request)
{
    (void)request;
}

} // namespace webkernel
