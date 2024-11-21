#pragma once
#include <string>
#include <map>

namespace webkernel 
{
    class ATemplateEngine
    {
        public:
        ATemplateEngine(const std::string& template_path);
        virtual ~ATemplateEngine();

        void setVariable(const std::string& key, const std::string& value);
        std::string render() const;
    
        private:
        ATemplateEngine();
        ATemplateEngine(const ATemplateEngine&);
        ATemplateEngine& operator=(const ATemplateEngine&);
        void _loadTemplate(const std::string& template_path);

        std::string _template_path;
        std::string _template_content;
        std::map<std::string, std::string> _variable_map;
    };
} // namespace webkernel
