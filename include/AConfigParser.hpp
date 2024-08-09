#pragma once
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include "defines.hpp"

namespace webconfig
{

class AConfigParser
{
  public:
    AConfigParser(ConfigBlockLevel block_level);
    virtual ~AConfigParser();

    virtual void parse(std::ifstream &file_stream) = 0;

  protected:
    ConfigBlockLevel _block_level;
    bool _is_comment(const std::string& line);

  private:
    AConfigParser();
    AConfigParser(const AConfigParser& other);
    AConfigParser& operator=(const AConfigParser& other);
};

} // namespace webconfig
