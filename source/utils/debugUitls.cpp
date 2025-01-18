#include "debugUtils.hpp"

namespace utils
{

std::string replaceCRLF(std::string str)
{
    const std::string target = "\r\n";
    const std::string replacement = "␍\n";

    std::string::size_type pos = 0;
    while ((pos = str.find(target, pos)) != std::string::npos) {
        str.replace(pos, target.length(), replacement);
        pos += replacement.length();
    }

    return (str);
}

} // namespace utils
