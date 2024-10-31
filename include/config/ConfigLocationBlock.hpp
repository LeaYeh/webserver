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

    std::string route(void) const;
    std::vector<webshell::RequestMethod> limitExcept(void) const;
    std::string root(void) const;
    std::string index(void) const;
    std::string redirect(void) const;
    bool autoindex(void) const;
    std::string cgiExtension(void) const;
    std::string cgiPath(void) const;
    bool enableUpload(void) const;
    std::string uploadPath(void) const;

    std::string parse(std::ifstream& file_stream);
    void printConfig(void) const;

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

    void _parseConfigDirective(const std::string& line);
    std::vector<webshell::RequestMethod>
    _parseLimitExcept(const std::string& line, const std::string& directive);
    bool _parseAutoindex(const std::string& line, const std::string& directive);
    bool _parseEnableUpload(const std::string& line,
                            const std::string& directive);
};

} // namespace webconfig
