#pragma once

#include "AConfigParser.hpp"
#include "ErrorPage.hpp"
#include "defines.hpp"
#include <cstddef>
#include <vector>

namespace webconfig
{

class ConfigHttpBlock : public AConfigParser
{
  public:
    ConfigHttpBlock();
    ConfigHttpBlock(const ConfigHttpBlock& other);
    ConfigHttpBlock& operator=(const ConfigHttpBlock& other);
    ~ConfigHttpBlock();

    std::string parse(std::ifstream& file_stream);
    void printConfig(void) const;
    size_t clientMaxBodySize(void) const;
    webshell::ContentType defaultType(void) const;
    std::vector<ErrorPage> errorPages(void) const;
    std::string autoindexPage(void) const;

  private:
    size_t _client_max_body_size;
    webshell::ContentType _default_type;
    std::vector<ErrorPage> _error_page_list;
    std::string _autoindex_page;

    void _parseConfigDirective(const std::string& line);
    size_t _parseClientMaxBodySize(const std::string& line);
    webshell::ContentType _parseDefaultType(const std::string& line);
    ErrorPage _parseErrorPage(const std::string& line);
    std::string _parseAutoindexPage(const std::string& line);
};

} // namespace webconfig
