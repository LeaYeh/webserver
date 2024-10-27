#include "ConfigLocationBlock.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include "utils/utils.hpp"

namespace webconfig
{

ConfigLocationBlock::ConfigLocationBlock()
    : AConfigParser(LOCATION), _route("/"), _root(""), _index("index.html"),
      _redirect(""), _autoindex(false), _cgi_extension(""), _cgi_path(""),
      _enable_upload(false), _upload_path("")
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

ConfigLocationBlock::ConfigLocationBlock(const ConfigLocationBlock& other)
    : AConfigParser(other), _route(other._route),
      _limit_except(other._limit_except), _root(other._root),
      _index(other._index), _redirect(other._redirect),
      _autoindex(other._autoindex), _cgi_extension(other._cgi_extension),
      _cgi_path(other._cgi_path), _enable_upload(other._enable_upload),
      _upload_path(other._upload_path)
{
}

ConfigLocationBlock&
ConfigLocationBlock::operator=(const ConfigLocationBlock& other)
{
    if (this != &other)
    {
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

ConfigLocationBlock::~ConfigLocationBlock()
{
}

std::string ConfigLocationBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        if (_needToSkip(line))
            continue;
        if (!_isValidSentence(line))
            throw std::runtime_error("Invalid sentence in location block " +
                                     line);
        if (_isScopeSymbol(line))
            return (line);
        _parseConfigDirective(line);
    }

    return ("");
}

void ConfigLocationBlock::printConfig(void) const
{
    weblog::Logger::log(weblog::DEBUG, "\troute: " + _route);
    weblog::Logger::log(weblog::DEBUG, "\tlimit_except:");
    for (std::size_t i = 0; i < _limit_except.size(); ++i)
        weblog::Logger::log(
            weblog::DEBUG,
            "\t\t" + utils::request_method_to_string(_limit_except[i]));
    weblog::Logger::log(weblog::DEBUG, "\troot: " + _root);
    weblog::Logger::log(weblog::DEBUG, "\tindex: " + _index);
    weblog::Logger::log(weblog::DEBUG,
                        "\tautoindex: " + utils::toString(_autoindex));
    weblog::Logger::log(weblog::DEBUG, "\tcgi_extension: " + _cgi_extension);
    weblog::Logger::log(weblog::DEBUG, "\tcgi_path: " + _cgi_path);
    weblog::Logger::log(weblog::DEBUG,
                        "\tenable_upload: " + utils::toString(_enable_upload));
    weblog::Logger::log(weblog::DEBUG, "\tupload_path: " + _upload_path);
}

void ConfigLocationBlock::_parseConfigDirective(const std::string& line)
{
    std::string directive = _getDirectiveName(line);

    if (directive == "route")
        _route = extract_directive_value(line, directive);
    else if (directive == "limit_except")
        _limit_except = _parseLimitExcept(line, directive);
    else if (directive == "root")
        _root = extract_directive_value(line, directive);
    else if (directive == "index")
        _index = extract_directive_value(line, directive);
    else if (directive == "redirect")
        _redirect = extract_directive_value(line, directive);
    else if (directive == "autoindex")
        _autoindex = _parseAutoindex(line, directive);
    else if (directive == "cgi_extension")
        _cgi_extension = extract_directive_value(line, directive);
    else if (directive == "cgi_path")
        _cgi_path = extract_directive_value(line, directive);
    else if (directive == "enable_upload")
        _enable_upload = _parseEnableUpload(line, directive);
    else if (directive == "upload_path")
        _upload_path = extract_directive_value(line, directive);
    else
        throw std::runtime_error("Invalid directive in location block: " +
                                 directive);
}

std::vector<webshell::RequestMethod>
ConfigLocationBlock::_parseLimitExcept(const std::string& line,
                                       const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);
    std::vector<std::string> method_str_list = utils::split(value, ' ');
    std::vector<webshell::RequestMethod> methods;

    for (std::size_t i = 0; i < method_str_list.size(); ++i)
        methods.push_back(string_to_request_method(method_str_list[i]));

    return (methods);
}

bool ConfigLocationBlock::_parseAutoindex(const std::string& line,
                                          const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    if (value == "on")
        return (true);
    else if (value == "off")
        return (false);
    throw std::runtime_error("Invalid value for autoindex: " + value);
}

bool ConfigLocationBlock::_parseEnableUpload(const std::string& line,
                                             const std::string& directive)
{
    std::string value = extract_directive_value(line, directive);

    if (value == "on")
        return (true);
    else if (value == "off")
        return (false);

    throw std::runtime_error("Invalid value for enable_upload: " + value);
}

} // namespace webconfig
