#include "ConfigGlobalBlock.hpp"

namespace webconfig
{

ConfigGlobalBlock::ConfigGlobalBlock()
    : AConfigParser(GLOBAL), _worker_processes(1), _worker_connections(1024)
{
    _valid_directives.insert("worker_processes");
    _valid_directives.insert("worker_connections");
}

ConfigGlobalBlock::~ConfigGlobalBlock()
{
}

unsigned int ConfigGlobalBlock::worker_processes(void) const
{
    return (_worker_processes);
}

unsigned int ConfigGlobalBlock::worker_connections(void) const
{
    return (_worker_connections);
}

void ConfigGlobalBlock::print_config(void) const
{
    std::cout << "Global scope:" << std::endl;
    std::cout << "\tworker_processes: " << _worker_processes << std::endl;
    std::cout << "\tworker_connections: " << _worker_connections << std::endl;
}

std::string ConfigGlobalBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        // std::cout << "GLOBAL line: " << line << std::endl;
        if (_need_to_skip(line))
            continue;
        if (!_is_valid_sentence(line))
            throw std::invalid_argument("Global scope has invalid line: " +
                                        line);
        if (_is_scope_symbol(line))
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
        _worker_processes =
            utils::stoi(extract_directive_value(line, directive));
    else if (directive == "worker_connections")
        _worker_connections =
            utils::stoi(extract_directive_value(line, directive));
}

} // namespace webconfig
