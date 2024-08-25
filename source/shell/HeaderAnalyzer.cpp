#include "HeaderAnalyzer.hpp"

namespace webshell
{
HeaderAnalyzer::HeaderAnalyzer()
{
}

HeaderAnalyzer::HeaderAnalyzer(const HeaderAnalyzer& src)
{
    (void)src;
}

HeaderAnalyzer& HeaderAnalyzer::operator=(const HeaderAnalyzer& src)
{
    if (this != &src)
    {
        HeaderAnalyzer::operator=(src);
    }
    return (*this);
}

HeaderAnalyzer::~HeaderAnalyzer() {}
} // namespace webshell
