#pragma once

#include "AConfigParser.hpp"
#include "ErrorPage.hpp"
#include "defines.hpp"
#include <set>
#include <vector>

namespace webconfig
{

class ConfigHttpBlock : public AConfigParser
{
  public:
    const static std::set<std::string> VALID_DIRECTIVES;
    ConfigHttpBlock();
    ~ConfigHttpBlock();

    std::string parse(std::ifstream& file_stream);
    void print_config(void) const;

  private:
    ConfigHttpBlock(const ConfigHttpBlock& other);
    ConfigHttpBlock& operator=(const ConfigHttpBlock& other);

    unsigned int _client_max_body_size;
    webshell::ContentType _default_type;
    std::vector<ErrorPage> _error_page_list;

    void _parse_config_directive(const std::string& line);
    unsigned int _parse_client_max_body_size(const std::string& line);
    webshell::ContentType _parse_default_type(const std::string& line);
    ErrorPage _parse_error_page(const std::string& line);
};

} // namespace webconfig
