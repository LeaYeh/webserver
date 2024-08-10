#pragma once
#include "config_utils.hpp"
#include "defines.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace webconfig
{

class AConfigParser
{
  public:
    AConfigParser(ConfigBlockLevel block_level);
    virtual ~AConfigParser();

    virtual std::string parse(std::ifstream& file_stream) = 0;

  protected:
    ConfigBlockLevel _block_level;
    bool _need_to_skip(const std::string& line) const;
    bool _is_valid_scentence(const std::string& line) const;
    std::set<std::string> _valid_directives;
    std::string _get_directive_name(const std::string& line) const;
    virtual bool _is_valid_directive(const std::string& directive) const;
    virtual void _parse_config_directive(const std::string& line) = 0;

  private:
    AConfigParser();
    AConfigParser(const AConfigParser& other);
    AConfigParser& operator=(const AConfigParser& other);
};

} // namespace webconfig
