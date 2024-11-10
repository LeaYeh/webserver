#pragma once
#include "ErrorPage.hpp"
#include "defines.hpp"
#include <string>
#include <vector>

namespace webconfig
{

struct RequestConfig
{
    // Http level configuration
    unsigned int client_max_body_size;
    std::string default_type;
    std::vector<ErrorPage> error_pages;

    // Server level configuration
    std::vector<std::pair<std::string, weblog::LogLevel> > error_log;
    unsigned int keep_alive_timeout;
    std::string server_name;

    // Location level configuration
    std::string route;
    std::vector<webshell::RequestMethod> limit_except;
    std::string root;
    std::string index;
    std::string redirect;
    bool autoindex;
    std::string cgi_extension;
    std::string cgi_path;
    bool enable_upload;
    std::string upload_path;
};

} // namespace webconfig
