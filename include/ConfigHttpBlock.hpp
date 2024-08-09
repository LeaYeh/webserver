#pragma once

#include "AConfigParser.hpp"
#include "defines.hpp"
#include <utility>
#include <vector>

namespace webconfig
{

class ConfigHttpBlock : public AConfigParser
{
  public:
    ConfigHttpBlock();
    ~ConfigHttpBlock();

    void parse(std::ifstream& file_stream);

  private:
    ConfigHttpBlock(const ConfigHttpBlock& other);
    ConfigHttpBlock& operator=(const ConfigHttpBlock& other);

    unsigned int _client_max_body_size;
    webshell::ContentType _default_type;
    std::vector<std::pair<webshell::StatusCode, std::string> > _error_page_list;
};

} // namespace webconfig
