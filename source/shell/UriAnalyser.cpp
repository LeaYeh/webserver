#include "UriAnalyser.hpp"

namespace webshell
{

UriAnalyser::UriAnalyser()
{
}

UriAnalyser::UriAnalyser(const UriAnalyser& other)
    : _scheme(other._scheme), _directory(other._directory), _query(other._query)
{
}

UriAnalyser& UriAnalyser::operator=(const UriAnalyser& other)
{
    if (this != &other)
    {
        _scheme = other._scheme;
        _directory = other._directory;
        _query = other._query;
    }
    return (*this);
}

UriAnalyser::~UriAnalyser()
{
}

void UriAnalyser::parse_uri(const std::string& uri)
{
    (void)uri;
}

std::string UriAnalyser::scheme() const
{
    return (_scheme);
}

std::string UriAnalyser::directory() const
{
    return (_directory);
}

std::string UriAnalyser::query() const
{
    return (_query);
}

} // namespace webshell
