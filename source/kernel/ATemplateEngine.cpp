#include "ATemplateEngine.hpp"
#include <fstream>
#include <sstream>

namespace webkernel
{
    ATemplateEngine::ATemplateEngine(const std::string& template_path)
    {
        _loadTemplate(template_path);
    }

    ATemplateEngine::~ATemplateEngine()
    {
    }

    void ATemplateEngine::setVariable(const std::string& key, const std::string& value)
    {
        _variable_map[key] = value;
    }

    std::string ATemplateEngine::render() const
    {
        std::string rendered_template = _template_content;
        for (std::map<std::string, std::string>::const_iterator it = _variable_map.begin(); it != _variable_map.end(); it++)
        {
            std::string key = "{{" + it->first + "}}";
            std::string value = it->second;
            size_t pos = rendered_template.find(key);
            while (pos != std::string::npos)
            {
                rendered_template.replace(pos, key.length(), value);
                pos = rendered_template.find(key);
            }
        }
        return (rendered_template);
    }

    void ATemplateEngine::_loadTemplate(const std::string& template_path)
    {
        _template_path = template_path;
        std::ifstream template_file(template_path.c_str());

        if (!template_file.is_open())
            throw std::runtime_error("Could not open template file: " + template_path);
        std::stringstream buffer;
        buffer << template_file.rdbuf();
        _template_content = buffer.str();
    }
} // namespace webkernel
