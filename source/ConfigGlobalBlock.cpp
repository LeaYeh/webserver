#include "ConfigGlobalBlock.hpp"

namespace webconfig
{

ConfigGlobalBlock::ConfigGlobalBlock()
    : AConfigParser(GLOBAL), worker_processes(1), worker_connections(1024)
{
    _valid_directives.insert("worker_processes");
    _valid_directives.insert("worker_connections");
}

ConfigGlobalBlock::~ConfigGlobalBlock()
{
}

std::string ConfigGlobalBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        if (!_is_valid_scentence(line))
            throw std::invalid_argument("Global scope has invalid line: " +
                                        line);
        if (_need_to_skip(line))
            continue;
        if (line.find("{") != std::string::npos)
            return (line);
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive))
            throw std::invalid_argument("Global scope has invalid directive: " +
                                        directive);
        _parse_config_directive(line);
    }
    return ("");
}

void ConfigGlobalBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "worker_processes")
        worker_processes =
            utils::stoi(extract_directive_value(line, directive));
    else if (directive == "worker_connections")
        worker_connections =
            utils::stoi(extract_directive_value(line, directive));
}

} // namespace webconfig
