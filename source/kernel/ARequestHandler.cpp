#include "ARequestHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <string>

namespace webkernel
{

ARequestHandler::ARequestHandler() : _is_cgi(false) {}

ARequestHandler::ARequestHandler(const ARequestHandler& other) :
    _is_cgi(other._is_cgi), _response_headers(other._response_headers)
{
}

ARequestHandler& ARequestHandler::operator=(const ARequestHandler& other)
{
    if (this != &other) {
        _is_cgi = other._is_cgi;
        _response_headers = other._response_headers;
    }
    return (*this);
}

ARequestHandler::~ARequestHandler() {}

bool ARequestHandler::_check_method_limit(
    webshell::RequestMethod method,
    const std::vector<webshell::RequestMethod>& limit) const
{
    for (std::size_t i = 0; i < limit.size(); ++i) {
        if (method == limit[i]) {
            return (true);
        }
    }
    return (false);
}

bool ARequestHandler::_check_path_format(const std::string& path) const
{
    if (path.size() > MAX_PATH_LENGTH) {
        return (false);
    }
    if (path.find("~") != std::string::npos) {
        return (false);
    }
    return (true);
}

bool ARequestHandler::_check_path_permission(const std::string& path,
                                             const int type) const
{
    if (access(path.c_str(), type) == -1) {
        return (false);
    }
    return (true);
}

bool ARequestHandler::_is_out_of_max_file_size(
    const webconfig::RequestConfig& config,
    const std::string file_path) const
{
    struct stat file_stat;

    if (stat(file_path.c_str(), &file_stat) == -1 || file_stat.st_size < 0) {
        throw std::runtime_error("ARequestHandler: failed to get file stat");
    }
    if ((size_t)file_stat.st_size > config.client_max_body_size) {
        return (true);
    }
    return (false);
}

const std::string
ARequestHandler::_get_mime_type(const std::string& file_path) const
{
    std::string extension = file_path.substr(file_path.find_last_of(".") + 1);

    if (extension == "html" || extension == "htm") {
        return ("text/html");
    }
    if (extension == "css") {
        return ("text/css");
    }
    if (extension == "js") {
        return ("text/javascript");
    }
    if (extension == "jpeg" || extension == "jpg") {
        return ("image/jpeg");
    }
    if (extension == "png") {
        return ("image/png");
    }
    if (extension == "gif") {
        return ("image/gif");
    }
    if (extension == "svg") {
        return ("image/svg+xml");
    }
    if (extension == "ico") {
        return ("image/x-icon");
    }
    if (extension == "json") {
        return ("application/json");
    }
    if (extension == "pdf") {
        return ("application/pdf");
    }
    if (extension == "zip") {
        return ("application/zip");
    }
    if (extension == "tar") {
        return ("application/x-tar");
    }
    if (extension == "gz") {
        return ("application/gzip");
    }
    if (extension == "mp3") {
        return ("audio/mpeg");
    }
    if (extension == "wav") {
        return ("audio/wav");
    }
    if (extension == "mp4") {
        return ("video/mp4");
    }
    if (extension == "webm") {
        return ("video/webm");
    }
    if (extension == "ogg") {
        return ("video/ogg");
    }
    return ("application/octet-stream");
}

std::string ARequestHandler::_get_encoding_string(int encoding) const
{
    std::string encoding_string = "identity";

    if (encoding & webkernel::GZIP) {
        encoding_string = "gzip";
    }
    if (encoding & webkernel::CHUNKED) {
        encoding_string += ",chunked";
    }
    return (encoding_string);
}

int ARequestHandler::_get_respones_encoding(
    const webshell::Request& request) const
{
    int encoding = webkernel::IDENTITY;
    const webconfig::RequestConfig& config = request.config();

    if (request.method() == webshell::GET
        && _is_out_of_max_file_size(config, config.root + request.uri().path)) {
        encoding |= webkernel::CHUNKED;
    }
    if (request.has_header("transfer-encoding")) {
        std::string transfer_encoding = request.get_header("transfer-encoding");
        if (transfer_encoding.find("chunked") != std::string::npos) {
            encoding |= webkernel::CHUNKED;
        }
        if (transfer_encoding.find("gzip") != std::string::npos) {
            encoding |= webkernel::GZIP;
        }
    }
    return (encoding);
}

std::string
ARequestHandler::_get_resource_path(const webconfig::RequestConfig& config,
                                    const std::string& request_path) const
{
    std::string full_path;

    // the root might be includeed in the path
    full_path = request_path;
    if (!utils::start_with(request_path, config.root)) {
        full_path = config.root + request_path;
    }
    // Check if the path is a directory or a file
    if (utils::is_directory(full_path)) {
        full_path = utils::join(full_path, config.index);
    }
    return (full_path);
}

void ARequestHandler::_handle_exception(
    const std::exception& e,
    const webshell::StatusCode code,
    const webshell::ContentType content_type)
{
    LOG(weblog::ERROR, e.what());
    throw utils::HttpException(code, e.what(), content_type);
}

void ARequestHandler::_update_status(EventProcessingState& state,
                                     EventProcessingState flags,
                                     bool overwrite) const
{
    if (overwrite) {
        state = flags;
    }
    else {
        state = static_cast<EventProcessingState>(state | flags);
    }
    LOG(weblog::DEBUG,
                        "ARequestHandler: update status to "
                            + explain_event_processing_state(state));
}

void ARequestHandler::_pre_process(const webshell::Request& request)
{
    if (!_check_path_format(request.uri().path)) {
        throw utils::HttpException(webshell::BAD_REQUEST, "Bad request");
    }
    if (!_check_method_limit(request.method(), request.config().limit_except)) {
        throw utils::HttpException(webshell::METHOD_NOT_ALLOWED,
                                   "Method not allowed");
    }
    // std::string session_id = request.get_cookie("session_id");

    // if (session_id.empty()) {
    //     session_id = uuid();
    //     weblog::Logger::log(weblog::DEBUG,
    //                         "Create new session id: " + session_id);
    // }

}

} // namespace webkernel
