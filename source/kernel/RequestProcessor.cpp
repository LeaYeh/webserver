#include "RequestProcessor.hpp"
#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConnectionHandler.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "RequestHandlerManager.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include "utils.hpp"
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
    {
        _analyzer_pool[fd] = webshell::RequestAnalyzer();
        _state[fd] = INITIAL;
    }

    while (i < buffer.size())
    {
        _analyzer_pool[fd].feed(buffer[i]);
        if (_analyzer_pool[fd].isComplete() /* && (i < buffer.size() - 1)*/)
        {
            _request_records[fd] = _analyzer_pool[fd].request();
            weblog::Logger::log(weblog::WARNING,
                                "Request is complete: \n" +
                                    _request_records[fd].serialize());
            setupRequestConfig(fd, _request_records[fd]);
            process(fd);
            buffer.erase(0, i + 1);
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
    webshell::Response response = manager->handleRequest(
        fd, state, _request_config_pool[fd], _request_records[fd]);
    weblog::Logger::log(weblog::DEBUG, "state: " + utils::toString(state));
    _handler->prepareWrite(fd, response.serialize());
    _reactor->modifyHandler(fd, EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);

    if (state & COMPELETED)
        _request_records.erase(fd);
    // TODO: After processing the request, we need to reset the analyzer or when
    // it is times out we need to remove it
    _analyzer_pool[fd].reset();
}

void RequestProcessor::removeAnalyzer(int fd)
{
    _analyzer_pool.erase(fd);
}

const webconfig::RequestConfig& RequestProcessor::requestConfig(int fd) const
{
    std::map<int /* fd */, webconfig::RequestConfig>::const_iterator it =
        _request_config_pool.find(fd);
    if (it == _request_config_pool.end())
        throw std::runtime_error("No request config found for fd: " +
                                 utils::toString(fd));
    return (it->second);
}

void RequestProcessor::setupRequestConfig(int fd,
                                          const webshell::Request& request)
{
    int server_id = _reactor->lookupServerId(fd);
    webconfig::ConfigHttpBlock http_config =
        webconfig::Config::instance()->httpBlock();
    webconfig::ConfigServerBlock server_config =
        webconfig::Config::instance()->serverBlockList()[server_id];
    _request_config_pool[fd] = webconfig::RequestConfig();

    _request_config_pool[fd].client_max_body_size =
        http_config.clientMaxBodySize();
    _request_config_pool[fd].default_type = http_config.defaultType();
    _request_config_pool[fd].error_pages = http_config.errorPages();
    _request_config_pool[fd].autoindex_page = http_config.autoindexPage();
    _request_config_pool[fd].error_log = server_config.errorLog();
    _request_config_pool[fd].keep_alive_timeout =
        server_config.keepAliveTimeout();
    _request_config_pool[fd].server_name = server_config.serverName();

    for (std::size_t i = 0; i < server_config.locationBlockList().size(); i++)
    {
        if (request.uri().path.find(
                server_config.locationBlockList()[i].route()) == 0)
        {
            _request_config_pool[fd].route =
                server_config.locationBlockList()[i].route();
            _request_config_pool[fd].limit_except =
                server_config.locationBlockList()[i].limitExcept();
            _request_config_pool[fd].root =
                server_config.locationBlockList()[i].root();
            _request_config_pool[fd].index =
                server_config.locationBlockList()[i].index();
            _request_config_pool[fd].redirect =
                server_config.locationBlockList()[i].redirect();
            _request_config_pool[fd].autoindex =
                server_config.locationBlockList()[i].autoindex();
            _request_config_pool[fd].cgi_extension =
                server_config.locationBlockList()[i].cgiExtension();
            _request_config_pool[fd].cgi_path =
                server_config.locationBlockList()[i].cgiPath();
            _request_config_pool[fd].enable_upload =
                server_config.locationBlockList()[i].enableUpload();
            _request_config_pool[fd].upload_path =
                server_config.locationBlockList()[i].uploadPath();
        }
    }
}

EventProcessingState& RequestProcessor::state(int fd)
{
    std::map<int /* fd */, EventProcessingState>::iterator it = _state.find(fd);
    if (it == _state.end())
        throw std::runtime_error("No state found for fd: " +
                                 utils::toString(fd));
    return (it->second);
}

} // namespace webkernel
