#pragma once

#include "AConfigParser.hpp"
#include "defines.hpp"
#include <string>
#include <vector>

// TODO: Need to support alias but not just root
// TODO: Consider to change the format to be more like nginx?
namespace webconfig
{

class ConfigLocationBlock : public AConfigParser
{
public:
    std::string route(void) const;
    std::vector<webshell::RequestMethod> limit_except(void) const;
    std::string root(void) const;
    std::string alias(void) const;
    std::string index(void) const;
    std::string redirect(void) const;
    bool autoindex(void) const;
    std::string cgi_extension(void) const;
    std::string cgi_path(void) const;
    bool enable_upload(void) const;

    std::string parse(std::ifstream& file_stream);
    void print_config(void) const;

public:
    ConfigLocationBlock();
    ConfigLocationBlock(const ConfigLocationBlock& other);
    ConfigLocationBlock& operator=(const ConfigLocationBlock& other);
    ~ConfigLocationBlock();
    bool operator<(const ConfigLocationBlock& other) const;

private:
    std::string _route;
    std::vector<webshell::RequestMethod> _limit_except;
    std::string _root;
    std::string _alias;
    std::string _index;
    std::string _redirect;
    bool _autoindex;
    std::string _cgi_extension;
    std::string _cgi_path;
    bool _enable_upload;

private:
    void _parse_config_directive(const std::string& line);
    std::vector<webshell::RequestMethod>
    _parse_limit_except(const std::string& line, const std::string& directive);
    bool _parse_autoindex(const std::string& line,
                          const std::string& directive);
    bool _parse_enable_upload(const std::string& line,
                              const std::string& directive);
    void _valid(void) const;
};

} // namespace webconfig
