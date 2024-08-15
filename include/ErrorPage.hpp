#pragma once

#include "defines.hpp"
#include "utils.hpp"

namespace webconfig
{

struct ErrorPage
{
    ErrorPage(webshell::StatusCode status_code, const std::string& path);
    ErrorPage(const ErrorPage& other);
    ErrorPage& operator=(const ErrorPage& other);
    ~ErrorPage();

    webshell::StatusCode status_code;
    std::string path;
};

} // namespace webconfig
