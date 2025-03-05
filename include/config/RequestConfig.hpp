#pragma once
#include "defines.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace webconfig
{

struct RequestConfig
{
    // Http level configuration
    size_t client_max_body_size;
    std::string default_type;
    std::string autoindex_page;

    // Server level configuration
    std::vector<std::pair<std::string, weblog::LogLevel> > error_log;
    size_t keep_alive_timeout;
    std::string server_name;

    // Location level configuration
    std::string route;
    std::vector<webshell::RequestMethod> limit_except;
    std::string root;
    std::string alias;
    std::string index;
    std::string redirect;
    bool autoindex;
    std::string cgi_extension;
    std::string cgi_path;
    bool enable_upload;
};

} // namespace webconfig
