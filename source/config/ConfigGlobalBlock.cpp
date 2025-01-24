#include "ConfigGlobalBlock.hpp"
#include "Logger.hpp"
#include "configUtils.hpp"
#include "utils.hpp"

namespace webconfig
{

ConfigGlobalBlock::ConfigGlobalBlock() :
    AConfigParser(GLOBAL),
    _worker_processes(1),
    _worker_connections(1024),
    _session_socket_path("/tmp/session.sock"),
    _session_expire_seconds(3600),
    _session_max_connections(1000)
{
    _valid_directives.insert("worker_processes");
    _valid_directives.insert("worker_connections");
    _valid_directives.insert("session_socket_path");
    _valid_directives.insert("session_expire_seconds");
    _valid_directives.insert("session_max_connections");
}

ConfigGlobalBlock::ConfigGlobalBlock(const ConfigGlobalBlock& other) :
    AConfigParser(other),
    _worker_processes(other._worker_processes),
    _worker_connections(other._worker_connections)
{
}

ConfigGlobalBlock& ConfigGlobalBlock::operator=(const ConfigGlobalBlock& other)
{
    if (this != &other) {
        AConfigParser::operator=(other);
        _worker_processes = other._worker_processes;
        _worker_connections = other._worker_connections;
    }
    return (*this);
}

ConfigGlobalBlock::~ConfigGlobalBlock() {}

size_t ConfigGlobalBlock::worker_processes(void) const
{
    return (_worker_processes);
}

size_t ConfigGlobalBlock::worker_connections(void) const
{
    return (_worker_connections);
}

void ConfigGlobalBlock::print_config(void) const
{
    weblog::Logger::log(weblog::DEBUG, "Global scope:");
    weblog::Logger::log(weblog::DEBUG,
                        "\tworker_processes: "
                            + utils::to_string(_worker_processes));
    weblog::Logger::log(weblog::DEBUG,
                        "\tworker_connections: "
                            + utils::to_string(_worker_connections));
    weblog::Logger::log(weblog::DEBUG,
                        "\tsession_socket_path: " + _session_socket_path);
    weblog::Logger::log(weblog::DEBUG,
                        "\tsession_expire_seconds: "
                            + utils::to_string(_session_expire_seconds));
    weblog::Logger::log(weblog::DEBUG,
                        "\tsession_max_connections: "
                            + utils::to_string(_session_max_connections));
}

const std::string& ConfigGlobalBlock::session_socket_path(void) const
{
    return (_session_socket_path);
}

size_t ConfigGlobalBlock::session_expire_seconds(void) const
{
    return (_session_expire_seconds);
}

size_t ConfigGlobalBlock::session_max_connections(void) const
{
    return (_session_max_connections);
}

std::string ConfigGlobalBlock::parse(std::ifstream& file_stream)
{
    std::string line;

    while (std::getline(file_stream, line)) {
        if (_need_to_skip(line)) {
            continue;
        }
        if (!_is_valid_sentence(line)) {
            throw std::invalid_argument("Global scope has invalid line: "
                                        + line);
        }
        if (_is_scope_symbol(line)) {
            return (line);
        }
        std::string directive = _get_directive_name(line);
        if (!_is_valid_directive(directive)) {
            throw std::invalid_argument("Global scope has invalid directive: "
                                        + directive);
        }
        _parse_config_directive(line);
    }
    return ("");
}

void ConfigGlobalBlock::_parse_config_directive(const std::string& line)
{
    std::string directive = _get_directive_name(line);

    if (directive == "worker_processes") {
        _worker_processes =
            utils::stoi(extract_directive_value(line, directive));
    }
    else if (directive == "worker_connections") {
        _worker_connections =
            utils::stoi(extract_directive_value(line, directive));
    }
    else if (directive == "session_socket_path") {
        _session_socket_path = extract_directive_value(line, directive);
    }
    else if (directive == "session_expire_seconds") {
        _session_expire_seconds =
            utils::stoi(extract_directive_value(line, directive));
    }
    else if (directive == "session_max_connections") {
        _session_max_connections =
            utils::stoi(extract_directive_value(line, directive));
    }
    else {
        throw std::invalid_argument("Invalid directive: " + directive);
    }
}

} // namespace webconfig
