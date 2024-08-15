#pragma once

#include "AConfigParser.hpp"
#include "defines.hpp"
#include <string>
#include <vector>

namespace webconfig
{

class ConfigLocationBlock : public AConfigParser
{
  public:
    ConfigLocationBlock();
    ConfigLocationBlock(const ConfigLocationBlock& other);
    ConfigLocationBlock& operator=(const ConfigLocationBlock& other);
    ~ConfigLocationBlock();

    std::string parse(std::ifstream& file_stream);
    void print_config(void) const;

  private:
    std::string _route;
    std::vector<webshell::RequestMethod> _limit_except;
    std::string _root;
    std::string _index;
    std::string _redirect;
    bool _autoindex;
    std::string _cgi_extension;
    std::string _cgi_path;
    bool _enable_upload;
    std::string _upload_path;

    void _parse_config_directive(const std::string& line);
    std::vector<webshell::RequestMethod> _parse_limit_except(const std::string& line, const std::string& directive);
    bool _parse_autoindex(const std::string& line, const std::string& directive);
    bool _parse_enable_upload(const std::string& line, const std::string& directive);

};

} // namespace webconfig
