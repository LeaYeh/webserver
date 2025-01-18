#pragma once
#include <map>
#include <string>

namespace webkernel
{
class TemplateEngine
{
public:
    void set_variable(const std::string& key, const std::string& value);
    void load_template(const std::string& template_path);
    std::string render() const;
    void reset();

public:
    TemplateEngine();
    ~TemplateEngine();

private:
    std::string _template_path;
    std::string _template_content;
    std::map<std::string, std::string> _variable_map;

private:
    TemplateEngine(const TemplateEngine&);
    TemplateEngine& operator=(const TemplateEngine&);
};
} // namespace webkernel
