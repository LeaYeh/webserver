#pragma once

#include "defines.hpp"
#include <string>

namespace webshell
{

class UriAnalyzer
{
  public:
    UriAnalyzer();
    UriAnalyzer(const UriAnalyzer& other);
    UriAnalyzer& operator=(const UriAnalyzer& other);
    ~UriAnalyzer();

    void parse_uri(const std::string& uri);

    std::string scheme(void) const;
    std::string directory(void) const;
    std::string query(void) const;

  private:
    std::string _scheme;
    std::string _directory;
    std::string _query;
};
} // namespace webshell
