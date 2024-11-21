#pragma once
#include <string>
#include <map>

namespace webkernel 
{
    class TemplateEngine
    {
        public:
        TemplateEngine(const std::string& template_path);
        ~TemplateEngine();

        void setVariable(const std::string& key, const std::string& value);
        std::string render() const;
    
        private:
        TemplateEngine();
        TemplateEngine(const TemplateEngine&);
        TemplateEngine& operator=(const TemplateEngine&);
        void _loadTemplate(const std::string& template_path);

        std::string _template_path;
        std::string _template_content;
        std::map<std::string, std::string> _variable_map;
    };
} // namespace webkernel
