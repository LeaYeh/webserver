#include "GetHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "ResponseBuilder.hpp"
#include "utils.hpp"
#include <string>

namespace webkernel
{
webshell::Response GetHandler::handle(const webconfig::RequestConfig& config,
                                      const webshell::Request& request)
{
    std::string target_path = _preProcess(config, request);
    std::stringstream content;

    weblog::Logger::log(weblog::DEBUG, "GetHandler handle: " + target_path);
    if (access(target_path.c_str(), R_OK) == -1)
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden no read permission on file: " +
                                       target_path);
    if (!config.redirect.empty())
        throw utils::HttpException(webshell::MOVED_PERMANENTLY,
                                   "Redirect to " + config.redirect);
    if (utils::isDirectory(target_path) && !config.autoindex)
        // _handle_autoindex(config, content);
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden autoindex is disabled");
    else if (_get_respones_encoding(config, request) & webkernel::CHUNKED)
        // _handle_chunked(config, content);
        throw utils::HttpException(webshell::NOT_IMPLEMENTED,
                                   "Chunked encoding is not implemented");
    else
        _handle_standard(target_path, content);

    // prepare headers in post process and encode the content if needed e.g.
    // gzip
    _postProcess(config, request, target_path, content);
    return (webshell::ResponseBuilder::buildResponse(
        webshell::OK, _response_headers, content.str()));
}

void GetHandler::_handle_standard(const std::string& target_path,
                                  std::stringstream& content)
{

    std::ifstream file(target_path.c_str(), std::ios::binary);

    if (!file.is_open())
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Open file failed");
    content << file.rdbuf();
    file.close();
}

} // namespace webkernel
