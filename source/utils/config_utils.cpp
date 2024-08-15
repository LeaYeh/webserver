#include "utils/config_utils.hpp"
#include "defines.hpp"

namespace webconfig
{

std::string extract_directive_value(const std::string& line,
                                    const std::string& directive)
{
    std::string value;
    std::string::size_type spos = line.find(directive) + directive.size() + 1;
    std::string::size_type epos = line.find(';', spos);

    if (spos == std::string::npos || epos == std::string::npos)
        throw std::invalid_argument(
            "extract_directive_value: invalid argument " + line);
    value = utils::trim(line.substr(spos, epos - spos));
    if (value.empty())
        throw std::invalid_argument(
            "extract_directive_value: invalid argument which is empty");
    return (value);
}

bool revert_one_line(std::ifstream& file_stream)
{
    file_stream.seekg(-1, std::ios_base::cur);
    char c;
    file_stream.get(c);
    while (c != '\n')
    {
        file_stream.seekg(-2, std::ios_base::cur);
        file_stream.get(c);
    }
    return (true);
}

webshell::ContentType string_to_content_type(const std::string& value)
{
    static std::map<std::string, webshell::ContentType> content_type_map;

    if (content_type_map.empty())
    {
        content_type_map["text/plain"] = webshell::TEXT_PLAIN;
        content_type_map["text/html"] = webshell::TEXT_HTML;
        content_type_map["text/css"] = webshell::TEXT_CSS;
        content_type_map["image/png"] = webshell::IMAGE_PNG;
        content_type_map["image/jpeg"] = webshell::IMAGE_JPEG;
        content_type_map["image/gif"] = webshell::IMAGE_GIF;
        content_type_map["application/json"] = webshell::APPLICATION_JSON;
    }

    std::map<std::string, webshell::ContentType>::const_iterator it =
        content_type_map.find(value);
    if (it == content_type_map.end())
        throw std::invalid_argument("string_to_content_type: invalid argument");
    return (it->second);
}

webshell::StatusCode string_to_status_code(const std::string& value)
{
    if (value.size() != 3)
        throw std::invalid_argument("string_to_status_code: invalid argument");

    try
    {
        webshell::StatusCode code =
            static_cast<webshell::StatusCode>(utils::stoi(value));
        return (code);
    }
    catch (const std::exception& e)
    {
        throw std::invalid_argument("string_to_status_code: invalid argument");
    }
}

weblog::LogLevel string_to_level(const std::string& value)
{
    static std::map<std::string, weblog::LogLevel> log_level_map;

    if (log_level_map.empty())
    {
        log_level_map["debug"] = weblog::DEBUG;
        log_level_map["info"] = weblog::INFO;
        log_level_map["warning"] = weblog::WARNING;
        log_level_map["error"] = weblog::ERROR;
        log_level_map["critical"] = weblog::CRITICAL;
    }

    std::map<std::string, weblog::LogLevel>::const_iterator it =
        log_level_map.find(value);
    if (it == log_level_map.end())
        throw std::invalid_argument("string_to_level: invalid argument");
    return (it->second);
}

webshell::RequestMethod string_to_request_method(const std::string& value)
{
    static std::map<std::string, webshell::RequestMethod> request_method_map;

    if (request_method_map.empty())
    {
        request_method_map["GET"] = webshell::GET;
        request_method_map["HEAD"] = webshell::HEAD;
        request_method_map["POST"] = webshell::POST;
        request_method_map["PUT"] = webshell::PUT;
        request_method_map["DELETE"] = webshell::DELETE;
    }

    std::map<std::string, webshell::RequestMethod>::const_iterator it =
        request_method_map.find(value);
    if (it == request_method_map.end())
        throw std::invalid_argument(
            "string_to_request_method: invalid argument");
    return (it->second);
}

std::string level_to_string(weblog::LogLevel level)
{
    static std::map<weblog::LogLevel, std::string> log_level_map;

    if (log_level_map.empty())
    {
        log_level_map[weblog::DEBUG] = "debug";
        log_level_map[weblog::INFO] = "info";
        log_level_map[weblog::WARNING] = "warning";
        log_level_map[weblog::ERROR] = "error";
        log_level_map[weblog::CRITICAL] = "critical";
    }

    std::map<weblog::LogLevel, std::string>::const_iterator it =
        log_level_map.find(level);
    if (it == log_level_map.end())
        throw std::invalid_argument("level_to_string: invalid argument");
    return (it->second);
}

} // namespace webconfig
