#include "ErrorPage.hpp"
#include "defines.hpp"

namespace webconfig
{

ErrorPage::ErrorPage(webshell::StatusCode status_code, const std::string& path)
    : status_code(status_code), path(path)
{
}

ErrorPage::ErrorPage(const ErrorPage& other)
    : status_code(other.status_code), path(other.path)
{
}

ErrorPage& ErrorPage::operator=(const ErrorPage& other)
{
    if (this != &other)
    {
        status_code = other.status_code;
        path = other.path;
    }
    return (*this);
}

ErrorPage::~ErrorPage()
{
}

} // namespace webconfig
