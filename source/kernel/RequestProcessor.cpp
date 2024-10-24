#include "RequestProcessor.hpp"
#include "ConnectionHandler.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include <cstddef>

namespace webkernel
{

RequestProcessor::RequestProcessor(ConnectionHandler* handler)
    : _handler(handler), _analyzer(), _builder()
{
}

RequestProcessor::RequestProcessor(const RequestProcessor& other)
    : _handler(other._handler), _analyzer(other._analyzer),
      _builder(other._builder)
{
}

RequestProcessor& RequestProcessor::operator=(const RequestProcessor& other)
{
    if (this != &other)
    {
        _handler = other._handler;
        _analyzer = other._analyzer;
        _builder = other._builder;
    }
    return (*this);
}

RequestProcessor::~RequestProcessor()
{
}

// Analyze the buffer and feed to the request analyzer char by char to avoid
// lossing data which belongs to the next request
// TODO: How to test this function?
void RequestProcessor::analyze(std::string& buffer)
{
    for (size_t i = 0; i < buffer.size(); i++)
    {
        _analyzer.feed(buffer[i]);
        if (_analyzer.isComplete() && (i < buffer.size() - 1))
        {
            buffer.erase(0, i);
            break;
        }
    }
}

bool RequestProcessor::isRequestComplete()
{
    return (_analyzer.isComplete());
}

void RequestProcessor::analyzeFinalize(int fd)
{
    if (true || _analyzer.hasError())
    {
        weblog::Logger::log(weblog::DEBUG,
                            "Error detected in request analyzer: " +
                                _analyzer.statusInfo().second);
        webshell::Response resp = _builder.buildErrorResponse(
            _analyzer.statusInfo().first, _analyzer.statusInfo().second);
        _handler->prepareWrite(fd, resp.serialize());
        return;
    }
    if (_analyzer.request().method() == webshell::GET)
        _processGet(_analyzer.request());
    else if (_analyzer.request().method() == webshell::POST)
        _processPost(_analyzer.request());
    else if (_analyzer.request().method() == webshell::PUT)
        _processPut(_analyzer.request());
    else if (_analyzer.request().method() == webshell::DELETE)
        _processDelete(_analyzer.request());
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
