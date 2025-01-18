#pragma once

#include "AConfigParser.hpp"
#include "defines.hpp"
#include <cstddef>

namespace webconfig
{

class ConfigHttpBlock : public AConfigParser
{
public:
    std::string parse(std::ifstream& file_stream);
    void print_config(void) const;
    size_t client_max_body_size(void) const;
    webshell::ContentType default_type(void) const;
    std::string autoindex_page(void) const;
    std::string error_page(void) const;

public:
    ConfigHttpBlock();
    ConfigHttpBlock(const ConfigHttpBlock& other);
    ConfigHttpBlock& operator=(const ConfigHttpBlock& other);
    ~ConfigHttpBlock();

private:
    size_t _client_max_body_size;
    webshell::ContentType _default_type;
    std::string _autoindex_page;
    std::string _error_page;

private:
    void _parse_config_directive(const std::string& line);
    size_t _parse_client_max_body_size(const std::string& line);
    webshell::ContentType _parse_default_type(const std::string& line);
};

} // namespace webconfig
