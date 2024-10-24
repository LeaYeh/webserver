#include "UriAnalyzer.hpp"

namespace webshell
{

UriAnalyzer::UriAnalyzer()
{
}

UriAnalyzer::UriAnalyzer(const UriAnalyzer& other)
    : _scheme(other._scheme), _directory(other._directory), _query(other._query)
{
}

UriAnalyzer& UriAnalyzer::operator=(const UriAnalyzer& other)
{
    if (this != &other)
    {
        _scheme = other._scheme;
        _directory = other._directory;
        _query = other._query;
    }
    return (*this);
}

UriAnalyzer::~UriAnalyzer()
{
}

void UriAnalyzer::parse_uri(const std::string& uri)
{
    (void)uri;
}

std::string UriAnalyzer::scheme() const
{
    return (_scheme);
}

std::string UriAnalyzer::directory() const
{
    return (_directory);
}

std::string UriAnalyzer::query() const
{
    return (_query);
}

} // namespace webshell
