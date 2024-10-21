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

void RequestProcessor::analyze(const char* buffer, size_t size)
{
    _analyzer.read(buffer, size);
}

bool RequestProcessor::isRequestComplete()
{
    return (_analyzer.isComplete());
}

void RequestProcessor::analyzeFinalize(int fd)
{
    if (_analyzer.state() < webshell::COMPLETE)
        return (_handleInvalid(fd));

    webshell::Request request = _analyzer.request();
    if (request.method() == webshell::GET)
        _processGet(request);
    else if (request.method() == webshell::POST)
        _processPost(request);
    else if (request.method() == webshell::PUT)
        _processPut(request);
    else if (request.method() == webshell::DELETE)
        _processDelete(request);
}

void RequestProcessor::_handleInvalid(int fd)
{
    weblog::logger.log(weblog::INFO, "Invalid request state detected: " +
                                             utils::toString(_analyzer.state()));
    webshell::Response resp = _builder.buildErrorResponse(
        webshell::BAD_REQUEST, );
    _handler->prepareWrite(fd, resp.serialize());
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
