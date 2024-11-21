#include "ARequestHandler.hpp"
#include "HttpException.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <string>

namespace webkernel
{

ARequestHandler::ARequestHandler()
{
}

ARequestHandler::ARequestHandler(const ARequestHandler& other)
    : _response_headers(other._response_headers)
{
}

ARequestHandler& ARequestHandler::operator=(const ARequestHandler& other)
{
    if (this != &other)
    {
        _response_headers = other._response_headers;
    }
    return (*this);
}

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

bool ARequestHandler::_is_out_of_max_file_size(
    const webconfig::RequestConfig& config, const std::string file_path) const
{
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1)
        return (true);
    if (file_stat.st_size > config.client_max_body_size)
        return (true);
    return (false);
}

const std::string
ARequestHandler::_getMimeType(const std::string& file_path) const
{
    std::string extension = file_path.substr(file_path.find_last_of(".") + 1);

    if (extension == "html" || extension == "htm")
        return ("text/html");
    if (extension == "css")
        return ("text/css");
    if (extension == "js")
        return ("text/javascript");
    if (extension == "jpeg" || extension == "jpg")
        return ("image/jpeg");
    if (extension == "png")
        return ("image/png");
    if (extension == "gif")
        return ("image/gif");
    if (extension == "svg")
        return ("image/svg+xml");
    if (extension == "ico")
        return ("image/x-icon");
    if (extension == "json")
        return ("application/json");
    if (extension == "pdf")
        return ("application/pdf");
    if (extension == "zip")
        return ("application/zip");
    if (extension == "tar")
        return ("application/x-tar");
    if (extension == "gz")
        return ("application/gzip");
    if (extension == "mp3")
        return ("audio/mpeg");
    if (extension == "wav")
        return ("audio/wav");
    if (extension == "mp4")
        return ("video/mp4");
    if (extension == "webm")
        return ("video/webm");
    if (extension == "ogg")
        return ("video/ogg");
    return ("application/octet-stream");
}

std::string ARequestHandler::_get_encoding_string(int encoding) const
{
    std::string encoding_string = "identity";

    if (encoding & webkernel::GZIP)
        encoding_string = "gzip";
    if (encoding & webkernel::CHUNKED)
        encoding_string += ",chunked";
    return (encoding_string);
}

int ARequestHandler::_get_respones_encoding(
    const webconfig::RequestConfig& config,
    const webshell::Request& request) const
{
    int encoding = webkernel::IDENTITY;

    if (_is_out_of_max_file_size(config, config.root + request.uri().path))
        encoding |= webkernel::CHUNKED;
    if (request.headers().find("Accept-Encoding") != request.headers().end())
    {
        std::string accept_encoding = request.headers().at("Accept-Encoding");
        if (accept_encoding.find("gzip") != std::string::npos)
            encoding |= webkernel::GZIP;
        if (accept_encoding.find("chunked") != std::string::npos)
            encoding |= webkernel::CHUNKED;
    }
    return (encoding);
}

std::string ARequestHandler::_preProcess(const webconfig::RequestConfig& config,
                                         const webshell::Request& request)
{

    if (!_checkPathFormat(request.uri().path))
        throw utils::HttpException(webshell::BAD_REQUEST, "Bad request");
    if (!_checkMethodLimit(request.method(), config.limit_except))
        throw utils::HttpException(webshell::METHOD_NOT_ALLOWED,
                                   "Method not allowed");

    std::string full_path;

    // the root might be includeed in the path
    full_path = request.uri().path;
    if (request.uri().path.find(config.root) == std::string::npos)
        full_path = config.root + request.uri().path;
    // Check if the path is a directory or a file
    if (utils::isDirectory(full_path))
    {
        if (full_path[full_path.size() - 1] != '/')
            full_path += "/";
        full_path += config.index;
    }
    if (full_path.find(config.root) == std::string::npos)
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "Forbidden out of root");
    return (full_path);
}

void ARequestHandler::_postProcess(const webconfig::RequestConfig& config,
                                   const webshell::Request& request,
                                   const std::string& target_path,
                                   const std::string& content)
{
    _response_headers["Content-Type"] = _getMimeType(target_path);
    if (_get_respones_encoding(config, request) & webkernel::CHUNKED)
    {
        int encoding = _get_respones_encoding(config, request);
        std::string tmp = _get_encoding_string(encoding);
        _response_headers["Transfer-Encoding"] = tmp;
    }
    else
        _response_headers["Content-Length"] = utils::toString(content.size());
}

} // namespace webkernel
