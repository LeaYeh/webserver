#include "ConfigLocationBlock.hpp"
#include "Logger.hpp"
#include "configUtils.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"

namespace webconfig
{

ConfigLocationBlock::ConfigLocationBlock() :
    AConfigParser(LOCATION),
    _route("/"),
    _root("./www/html"),
    _index(""),
    _redirect(""),
    _autoindex(true),
    _cgi_extension(""),
    _cgi_path(""),
    _enable_upload(false),
    _upload_path("")
{
    _limit_except.push_back(webshell::GET);
    _valid_directives.insert("route");
    _valid_directives.insert("limit_except");
    _valid_directives.insert("root");
    _valid_directives.insert("index");
    _valid_directives.insert("redirect");
    _valid_directives.insert("autoindex");
    _valid_directives.insert("cgi_extension");
    _valid_directives.insert("cgi_path");
    _valid_directives.insert("enable_upload");
    _valid_directives.insert("upload_path");
    _valid_directives.insert("redirect");
}

ConfigLocationBlock::ConfigLocationBlock(const ConfigLocationBlock& other) :
    AConfigParser(other),
    _route(other._route),
    _limit_except(other._limit_except),
    _root(other._root),
    _index(other._index),
    _redirect(other._redirect),
    _autoindex(other._autoindex),
    _cgi_extension(other._cgi_extension),
    _cgi_path(other._cgi_path),
    _enable_upload(other._enable_upload),
    _upload_path(other._upload_path)
{
}

ConfigLocationBlock&
ConfigLocationBlock::operator=(const ConfigLocationBlock& other)
{
    if (this != &other) {
        AConfigParser::operator=(other);
        _route = other._route;
        _limit_except = other._limit_except;
        _root = other._root;
        _index = other._index;
        _redirect = other._redirect;
        _autoindex = other._autoindex;
        _cgi_extension = other._cgi_extension;
        _cgi_path = other._cgi_path;
        _enable_upload = other._enable_upload;
        _upload_path = other._upload_path;
    }
    return (*this);
}

ConfigLocationBlock::~ConfigLocationBlock() {}

bool ConfigLocationBlock::operator<(const ConfigLocationBlock& other) const
{
    if (_route.length() != other._route.length()) {
        return (_route.length() > other._route.length());
    }
    return (_route < other._route);
}

std::string ConfigLocationBlock::route(void) const
{
    return (_route);
}

std::vector<webshell::RequestMethod>
ConfigLocationBlock::limit_except(void) const
{
    return (_limit_except);
}

std::string ConfigLocationBlock::root(void) const
{
    return (_root);
}

std::string ConfigLocationBlock::index(void) const
{
    return (_index);
}

std::string ConfigLocationBlock::redirect(void) const
{
    return (_redirect);
}

bool ConfigLocationBlock::autoindex(void) const
{
    return (_autoindex);
}

std::string ConfigLocationBlock::cgi_extension(void) const
{
    return (_cgi_extension);
}

std::string ConfigLocationBlock::cgi_path(void) const
{
    return (_cgi_path);
}

bool ConfigLocationBlock::enable_upload(void) const
{
    return (_enable_upload);
}

std::string ConfigLocationBlock::upload_path(void) const
{
    return (_upload_path);
}

std::string ConfigLocationBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line)) {
        if (_need_to_skip(line)) {
            continue;
        }
        if (!_is_valid_sentence(line)) {
            throw std::runtime_error("Invalid sentence in location block "
                                     + line);
        }
        if (_is_scope_symbol(line)) {
            return (line);
        }
        _parse_config_directive(line);
    }

    return ("");
}

void ConfigLocationBlock::print_config(void) const
{
    LOG(weblog::DEBUG, "\troute: " + _route);
    LOG(weblog::DEBUG, "\tlimit_except:");
    for (std::size_t i = 0; i < _limit_except.size(); ++i) {
        LOG(weblog::DEBUG,
            "\t\t" + webshell::request_method_to_string(_limit_except[i]));
    }
    LOG(weblog::DEBUG, "\troot: " + _root);
    LOG(weblog::DEBUG, "\tindex: " + _index);
    LOG(weblog::DEBUG, "\tautoindex: " + utils::to_string(_autoindex));
    LOG(weblog::DEBUG, "\tcgi_extension: " + _cgi_extension);
    LOG(weblog::DEBUG, "\tcgi_path: " + _cgi_path);
    LOG(weblog::DEBUG, "\tenable_upload: " + utils::to_string(_enable_upload));
    LOG(weblog::DEBUG, "\tupload_path: " + _upload_path);
}

void ConfigLocationBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "route") {
        _route = extract_directive_value(line, directive);
    }
    else if (directive == "limit_except") {
        _limit_except = _parse_limit_except(line, directive);
    }
    else if (directive == "root") {
        _root = extract_directive_value(line, directive);
    }
    else if (directive == "index") {
        _index = extract_directive_value(line, directive);
    }
    else if (directive == "redirect") {
        _redirect = extract_directive_value(line, directive);
    }
    else if (directive == "autoindex") {
        _autoindex = _parse_autoindex(line, directive);
    }
    else if (directive == "cgi_extension") {
        _cgi_extension = extract_directive_value(line, directive);
    }
    else if (directive == "cgi_path") {
        _cgi_path = extract_directive_value(line, directive);
    }
    else if (directive == "enable_upload") {
        _enable_upload = _parse_enable_upload(line, directive);
    }
    else if (directive == "upload_path") {
        _upload_path = extract_directive_value(line, directive);
    }
    else {
        throw std::runtime_error("Invalid directive in location block: "
                                 + directive);
    }
}

std::vector<webshell::RequestMethod>
ConfigLocationBlock::_parse_limit_except(const std::string& line,
                                         const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);
    std::vector<std::string> method_str_list = utils::split(value, ' ');
    std::vector<webshell::RequestMethod> methods;

    for (std::size_t i = 0; i < method_str_list.size(); ++i) {
        methods.push_back(string_to_request_method(method_str_list[i]));
    }

    return (methods);
}

bool ConfigLocationBlock::_parse_autoindex(const std::string& line,
                                           const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    if (value == "on") {
        return (true);
    }
    else if (value == "off") {
        return (false);
    }
    throw std::runtime_error("Invalid value for autoindex: " + value);
}

bool ConfigLocationBlock::_parse_enable_upload(const std::string& line,
                                               const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    if (value == "on") {
        return (true);
    }
    else if (value == "off") {
        return (false);
    }

    throw std::runtime_error("Invalid value for enable_upload: " + value);
}

} // namespace webconfig
