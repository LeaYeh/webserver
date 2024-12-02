#include "RequestProcessor.hpp"
#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigLocationBlock.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "RequestAnalyzer.hpp"
#include "RequestHandlerManager.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
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
        _analyzer_pool[fd] = webshell::RequestAnalyzer(&_request_config_pool[fd], &buffer); //TODO: import read buffer and CONST request config ref body limit
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
            if (!setupRequestConfig(fd, _request_records[fd]))
                throw utils::HttpException(
                    webshell::NOT_FOUND, "No matching location block found: " +
                                             _request_records[fd].uri().path);
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
    webshell::Response response = manager->handleRequest(
        fd, state, _request_config_pool[fd], _request_records[fd]);
    weblog::Logger::log(weblog::DEBUG,
                        "state: " + explainEventProcessingState(state));
    _handler->prepareWrite(fd, response.serialize());

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

bool RequestProcessor::setupRequestConfig(int fd,
                                          const webshell::Request& request)
{
    webconfig::ConfigLocationBlock* location_config = NULL;
    int server_id = _reactor->lookupServerId(fd);
    webconfig::ConfigHttpBlock http_config =
        webconfig::Config::instance()->httpBlock();
    webconfig::ConfigServerBlock server_config =
        webconfig::Config::instance()->serverBlockList()[server_id];

    for (std::size_t i = 0; i < server_config.locationBlockList().size(); i++)
    {
        if (utils::start_with(request.uri().path,
                              server_config.locationBlockList()[i].route()))
        {
            location_config = &server_config.locationBlockList()[i];
            break;
        }
    }
    if (location_config == NULL)
        return (false);

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

    _request_config_pool[fd].route = location_config->route();
    _request_config_pool[fd].limit_except = location_config->limitExcept();
    _request_config_pool[fd].root = location_config->root();
    _request_config_pool[fd].index = location_config->index();
    _request_config_pool[fd].redirect = location_config->redirect();
    _request_config_pool[fd].autoindex = location_config->autoindex();
    _request_config_pool[fd].cgi_extension = location_config->cgiExtension();
    _request_config_pool[fd].cgi_path = location_config->cgiPath();
    _request_config_pool[fd].enable_upload = location_config->enableUpload();
    _request_config_pool[fd].upload_path = location_config->uploadPath();

    return (true);
}

const EventProcessingState& RequestProcessor::state(int fd) const
{
    if (_state.find(fd) == _state.end())
        throw std::runtime_error("No state found for fd: " +
                                 utils::toString(fd));
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
