#include "RequestProcessor.hpp"
#include "Config.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "RequestHandlerManager.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include <exception>
#include <string>

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
        _analyzer_pool[fd] = webshell::RequestAnalyzer();
    while (i < buffer.size())
    {
        weblog::Logger::log(weblog::CRITICAL,
                            "Feed char: " + utils::toString(buffer[i]) +
                                " to analyzer on fd: " + utils::toString(fd));
        _analyzer_pool[fd].feed(buffer[i]);
        if (_analyzer_pool[fd].isComplete() && (i < buffer.size() - 1))
        {
            analyzeFinalize(fd);
            buffer.erase(0, i);
            return (true);
        }
        i++;
    }
    buffer.erase(0, i);
    return (_analyzer_pool[fd].isComplete());
}

void RequestProcessor::analyzeFinalize(int fd)
{
    int server_id = _reactor->lookupServerId(fd);
    webconfig::ConfigServerBlock server_config;

    try
    {
        server_config =
            webconfig::Config::instance()->serverBlockList()[server_id];
    }
    catch (std::exception const& e)
    {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR, e.what());
    }
    webshell::Uri uri;
    uri.path = "/";
    webshell::Request request;
    std::string target = "/";
    request.setMethod(webshell::GET);
    request.setUri(uri);
    request.setVersion(1.1);

    webshell::Response response =
        RequestHandlerManager::getInstance().handleRequest(
            webshell::GET, &server_config, request);
    _handler->prepareWrite(fd, response.serialize());

    // TODO: After processing the request, we need to reset the analyzer or when
    // it is times out we need to remove it
    _analyzer_pool[fd].reset();
}

void RequestProcessor::removeAnalyzer(int fd)
{
    _analyzer_pool.erase(fd);
}

// void RequestProcessor::_processGet(int fd, const webshell::Request& request)
// {
//     (void)request;
//     webshell::Response dummy_response;

//     dummy_response.setStatusCode(webshell::OK);
//     dummy_response.setBody("Hello, World!");
//     _handler->prepareWrite(fd, dummy_response.serialize());
// }

// void RequestProcessor::_processPost(int fd, const webshell::Request& request)
// {
//     (void)fd;
//     (void)request;
// }

// void RequestProcessor::_processPut(int fd, const webshell::Request& request)
// {
//     (void)fd;
//     (void)request;
// }

// void RequestProcessor::_processDelete(int fd, const webshell::Request&
// request)
// {
//     (void)fd;
//     (void)request;
// }

} // namespace webkernel
