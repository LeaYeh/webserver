#pragma once
#include "defines.hpp"
#include "utils/configUtils.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace webconfig
{

class AConfigParser
{
  public:
    AConfigParser();
    AConfigParser(ConfigBlockLevel block_level);
    AConfigParser(const AConfigParser& other);
    AConfigParser& operator=(const AConfigParser& other);
    virtual ~AConfigParser();

    virtual std::string parse(std::ifstream& file_stream) = 0;
    virtual void printConfig(void) const = 0;

  protected:
    ConfigBlockLevel _block_level;
    std::set<std::string> _valid_directives;
    bool _needToSkip(const std::string& line) const;
    bool _isValidSentence(const std::string& line) const;
    bool _isScopeSymbol(const std::string& line) const;
    std::string _getDirectiveName(const std::string& line) const;
    bool _isValidDirective(const std::string& directive) const;
    virtual void _parseConfigDirective(const std::string& line) = 0;

  private:
};

} // namespace webconfig
