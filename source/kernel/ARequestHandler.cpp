#include "ARequestHandler.hpp"
#include "defines.hpp"
#include <utility>

namespace webkernel
{
ARequestHandler::~ARequestHandler()
{
}

bool ARequestHandler::_checkMethodLimit(
    webshell::RequestMethod method,
    const std::vector<webshell::RequestMethod>& limit) const
{
    for (std::size_t i = 0; i < limit.size(); ++i)
    {
        if (method == limit[i])
            return (true);
    }
    return (false);
}

bool ARequestHandler::_checkPathFormat(const std::string& path) const
{
    // TODO: Need to check the path format with RFC3986, and which module should
    // implement this? URI Analyzer? if (path[0] != '/')
    //     return (false);
    // if (path.find("..") != std::string::npos)
    //     return (false);
    // if (path.find("//") != std::string::npos)
    //     return (false);
    if (path.size() > MAX_PATH_LENGTH)
        return (false);
    if (path.find("~") != std::string::npos)
        return (false);
    return (true);
}

std::pair<webshell::StatusCode, std::string>
ARequestHandler::_preProcess(const webconfig::RequestConfig& config,
                             const webshell::Request& request) const
{
    if (!_checkPathFormat(request.uri().path))
        return (std::make_pair(webshell::BAD_REQUEST, "Invalid path format"));
    if (!_checkMethodLimit(request.method(), config.limit_except))
        return (
            std::make_pair(webshell::METHOD_NOT_ALLOWED, "Method not allowed"));
    // Check if the path is a directory or a file
    std::string full_path = config.root + request.uri().path;
    if (utils::isDirectory(full_path))
        return (std::make_pair(webshell::FORBIDDEN, "Dir access forbidden"));
    // Check if the file has the right permission
    if (access(full_path.c_str(), R_OK) == -1)
        return (std::make_pair(webshell::FORBIDDEN, "File access forbidden"));

    return (std::make_pair(webshell::OK, ""));
}

} // namespace webkernel
