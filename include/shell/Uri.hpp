#pragma once
#include <string>

namespace webshell
{

struct Uri
{
    std::string raw;
    std::string scheme;
    std::string authority;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    std::string fragment;
    std::string userinfo;
};

} // namespace webshell
