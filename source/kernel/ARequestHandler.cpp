#include "ARequestHandler.hpp"
#include "Config.hpp"
#include "HttpException.hpp"
#include "defines.hpp"

namespace webkernel
{
ARequestHandler::~ARequestHandler()
{
}

bool ARequestHandler::checkMethodLimit(
    webshell::RequestMethod method,
    const std::vector<webshell::RequestMethod>& limit)
{
    for (std::size_t i = 0; i < limit.size(); ++i)
    {
        if (method == limit[i])
            return (true);
    }
    return (false);
}

bool ARequestHandler::checkPathFormat(const std::string& path)
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

webconfig::ConfigLocationBlock*
ARequestHandler::matchRequestPath(const webshell::Request& request,
                                  int server_id)
{
    webconfig::ConfigServerBlock server_config =
        webconfig::Config::instance()->serverBlockList()[server_id];
    std::string path = request.uri().path;

    // Check if the path is valid
    if (!checkPathFormat(path))
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Invalid path format");

    // Check if the path is in the location block
    for (std::size_t i = 0; i < server_config.locationBlockList().size(); ++i)
    {
        if (path.find(server_config.locationBlockList()[i].route()) == 0)
        {
            if (!checkMethodLimit(
                    request.method(),
                    server_config.locationBlockList()[i].limitExcept()))
                throw utils::HttpException(webshell::METHOD_NOT_ALLOWED,
                                           "Method not allowed");
            // Check if the path is a directory or a file
            std::string full_path =
                server_config.locationBlockList()[i].root() + path;
            if (utils::isDirectory(full_path))
                throw utils::HttpException(webshell::FORBIDDEN,
                                           "Directory access forbidden");
            // Check if the file has the right permission
            if (access(full_path.c_str(), R_OK) == -1)
                throw utils::HttpException(webshell::FORBIDDEN,
                                           "File access forbidden");
            return (&server_config.locationBlockList()[i]);
        }
    }
    throw utils::HttpException(webshell::NOT_FOUND, "Location not found");
}

} // namespace webkernel
