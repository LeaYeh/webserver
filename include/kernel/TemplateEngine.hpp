#pragma once
#include <string>
#include <map>

namespace webkernel
{
    class TemplateEngine
    {
        public:
        TemplateEngine();
        ~TemplateEngine();

        void setVariable(const std::string& key, const std::string& value);
        void loadTemplate(const std::string& template_path);
        std::string render() const;
        void reset();

        private:
        TemplateEngine(const TemplateEngine&);
        TemplateEngine& operator=(const TemplateEngine&);

        std::string _template_path;
        std::string _template_content;
        std::map<std::string, std::string> _variable_map;
    };
} // namespace webkernel
