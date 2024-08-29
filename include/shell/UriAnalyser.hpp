#pragma once

#include "defines.hpp"
#include <iostream>

namespace webshell
{

class UriAnalyser
{
  public:
    UriAnalyser();
    UriAnalyser(const UriAnalyser& other);
    UriAnalyser& operator=(const UriAnalyser& other);
    ~UriAnalyser();

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
