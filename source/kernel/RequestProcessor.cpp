#include "RequestProcessor.hpp"
#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConnectionHandler.hpp"
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

void RequestProcessor::setupRequestConfig(int fd,
                                          const webshell::Request& request)
{
    int server_id = _reactor->lookupServerId(fd);
    webconfig::ConfigHttpBlock http_config =
        webconfig::Config::instance()->httpBlock();
    webconfig::ConfigServerBlock server_config =
        webconfig::Config::instance()->serverBlockList()[server_id];

    _request_config.client_max_body_size = http_config.clientMaxBodySize();
    _request_config.default_type = http_config.defaultType();
    _request_config.error_pages = http_config.errorPages();
    _request_config.error_log = server_config.errorLog();
    _request_config.keep_alive_timeout = server_config.keepAliveTimeout();
    _request_config.server_name = server_config.serverName();

    for (std::size_t i = 0; i < server_config.locationBlockList().size(); i++)
    {
        if (request.uri().path.find(
                server_config.locationBlockList()[i].route()) == 0)
        {
            _request_config.route =
                server_config.locationBlockList()[i].route();
            _request_config.limit_except =
                server_config.locationBlockList()[i].limitExcept();
            _request_config.root = server_config.locationBlockList()[i].root();
            _request_config.index =
                server_config.locationBlockList()[i].index();
            _request_config.redirect =
                server_config.locationBlockList()[i].redirect();
            _request_config.autoindex =
                server_config.locationBlockList()[i].autoindex();
            _request_config.cgi_extension =
                server_config.locationBlockList()[i].cgiExtension();
            _request_config.cgi_path =
                server_config.locationBlockList()[i].cgiPath();
            _request_config.enable_upload =
                server_config.locationBlockList()[i].enableUpload();
            _request_config.upload_path =
                server_config.locationBlockList()[i].uploadPath();
        }
    }
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
        if (_analyzer_pool[fd].isComplete()/* && (i < buffer.size() - 1)*/)
        {
            process(fd);
            buffer.erase(0, i + 1); //because atp you have not incremented i yet
            return (true);
        }
        i++;
    }
    buffer.erase(0, i);
    return (_analyzer_pool[fd].isComplete());
}

void RequestProcessor::process(int fd)
{
    try
    {
        setupRequestConfig(fd, _analyzer_pool[fd].request());
    }
    catch (std::exception& e)
    {
        weblog::Logger::log(weblog::ERROR, e.what());
        _handler->prepareError(fd, webshell::INTERNAL_SERVER_ERROR, e.what());
        return;
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
            _analyzer_pool[fd].request().method(), _request_config, request);
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
