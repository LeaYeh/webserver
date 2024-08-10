#include "config_utils.hpp"
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
            "extract_directive_value: invalid argument");
    value = utils::trim(line.substr(spos, epos - spos));
    if (value.empty())
        throw std::invalid_argument(
            "extract_directive_value: invalid argument");
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

webshell::ContentType get_content_type(const std::string& value)
{
    static std::map<std::string, webshell::ContentType> content_type_map;

    if (content_type_map.empty()) {
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
        throw std::invalid_argument("get_content_type: invalid argument");
    return (it->second);
}

webshell::StatusCode convert_to_status_code(const std::string& value)
{
    if (value.size() != 3)
        throw std::invalid_argument("convert_to_status_code: invalid argument");

    try {
        webshell::StatusCode code = static_cast<webshell::StatusCode>(utils::stoi(value));
        return (code);
    } catch (const std::exception& e) {
        throw std::invalid_argument("convert_to_status_code: invalid argument");
    }
}

} // namespace webconfig
