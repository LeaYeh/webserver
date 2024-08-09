#pragma once

#include "AConfigParser.hpp"
#include "defines.hpp"
#include <vector>

namespace webconfig
{

class ConfigLocationBlock : public AConfigParser
{
  public:
    ConfigLocationBlock();
    ~ConfigLocationBlock();

    void parse(std::ifstream& file_stream);

  private:
    ConfigLocationBlock(const ConfigLocationBlock& other);
    ConfigLocationBlock& operator=(const ConfigLocationBlock& other);

    std::string _location;
    std::vector<webshell::RequestMethod> _limit_except;
    std::string _root;
    std::string _index;
    std::string _redirect;
    bool _autoindex;
    std::string _cgi_extension;
    std::string _cgi_path;
    bool _enable_upload;
    std::string _upload_path;
};

} // namespace webconfig
