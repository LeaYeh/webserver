#include "ConfigGlobalBlock.hpp"
#include "utils/Logger.hpp"

namespace webconfig
{

ConfigGlobalBlock::ConfigGlobalBlock()
    : AConfigParser(GLOBAL), _worker_processes(1), _worker_connections(1024)
{
    _valid_directives.insert("worker_processes");
    _valid_directives.insert("worker_connections");
}

ConfigGlobalBlock::ConfigGlobalBlock(const ConfigGlobalBlock& other)
    : AConfigParser(other), _worker_processes(other._worker_processes),
      _worker_connections(other._worker_connections)
{
}

ConfigGlobalBlock& ConfigGlobalBlock::operator=(const ConfigGlobalBlock& other)
{
    if (this != &other)
    {
        AConfigParser::operator=(other);
        _worker_processes = other._worker_processes;
        _worker_connections = other._worker_connections;
    }
    return (*this);
}

ConfigGlobalBlock::~ConfigGlobalBlock()
{
}

unsigned int ConfigGlobalBlock::workerProcesses(void) const
{
    return (_worker_processes);
}

unsigned int ConfigGlobalBlock::workerConnections(void) const
{
    return (_worker_connections);
}

void ConfigGlobalBlock::printConfig(void) const
{
    weblog::Logger::log(weblog::DEBUG, "Global scope:");
    weblog::Logger::log(weblog::DEBUG,
                           "\tworker_processes: " +
                               utils::toString(_worker_processes));
    weblog::Logger::log(weblog::DEBUG,
                           "\tworker_connections: " +
                               utils::toString(_worker_connections));
}

std::string ConfigGlobalBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line))
    {
        if (_needToSkip(line))
            continue;
        if (!_isValidSentence(line))
            throw std::invalid_argument("Global scope has invalid line: " +
                                        line);
        if (_isScopeSymbol(line))
            return (line);
        std::string directive = _getDirectiveName(line);
        if (!_isValidDirective(directive))
            throw std::invalid_argument("Global scope has invalid directive: " +
                                        directive);
        _parseConfigDirective(line);
    }
    return ("");
}

void ConfigGlobalBlock::_parseConfigDirective(const std::string& line)
{
    std::string directive = _getDirectiveName(line);

    if (directive == "worker_processes")
        _worker_processes =
            utils::stoi(extract_directive_value(line, directive));
    else if (directive == "worker_connections")
        _worker_connections =
            utils::stoi(extract_directive_value(line, directive));
}

} // namespace webconfig
