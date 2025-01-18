#pragma once
#include "defines.hpp"
#include <fstream>
#include <set>
#include <string>

namespace webconfig
{

class AConfigParser
{
public:
    virtual std::string parse(std::ifstream& file_stream) = 0;
    virtual void print_config(void) const = 0;

public:
    AConfigParser();
    AConfigParser(ConfigBlockLevel block_level);
    AConfigParser(const AConfigParser& other);
    AConfigParser& operator=(const AConfigParser& other);
    virtual ~AConfigParser();

protected:
    ConfigBlockLevel _block_level;
    std::set<std::string> _valid_directives;

protected:
    bool _need_to_skip(const std::string& line) const;
    bool _is_valid_sentence(const std::string& line) const;
    bool _is_scope_symbol(const std::string& line) const;
    std::string _get_directive_name(const std::string& line) const;
    bool _is_valid_directive(const std::string& directive) const;
    virtual void _parse_config_directive(const std::string& line) = 0;
};

} // namespace webconfig
