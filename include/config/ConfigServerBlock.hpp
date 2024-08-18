/**
 * @file ConfigServerBlock.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "AConfigParser.hpp"
#include "ConfigLocationBlock.hpp"
#include <fcntl.h>
#include <set>
#include <vector>

namespace webconfig
{

/**
 * @class ConfigServerBlock
 * @brief The ConfigServerBlock class is a concrete class for parsing the server block of the configuration file.
 */
class ConfigServerBlock : public AConfigParser
{
  public:
    /**
     * @brief Default constructor for ConfigServerBlock.
     */
    ConfigServerBlock();

    /**
     * @brief Copy constructor for ConfigServerBlock.
     * @param other The ConfigServerBlock object to be copied.
     */
    ~ConfigServerBlock();

    /**
     * @brief Copy constructor for ConfigServerBlock.
     * @param other The ConfigServerBlock object to be copied.
     */
    ConfigServerBlock(const ConfigServerBlock& other);

    /**
     * @brief Assignment operator for ConfigServerBlock.
     * @param other The ConfigServerBlock object to be assigned.
     * @return A reference to the assigned ConfigServerBlock object.
     */
    ConfigServerBlock& operator=(const ConfigServerBlock& other);

    /**
     * @brief Returns the server name.
     * @return The server name.
     */
    std::string server_name(void) const;

    /**
     * @brief Returns the listen directive.
     * @return The listen directive.
     */
    std::pair<std::string, std::string> listen(void) const;

    /**
     * @brief Returns the error log.
     * @return The error log.
     */
    std::vector<std::pair<std::string, weblog::LogLevel> > error_log(void) const;

    /**
     * @brief Returns the keepalive timeout.
     * @return The keepalive timeout.
     */
    unsigned int keepalive_timeout(void) const;

    /**
     * @brief Returns the location block list.
     * @return The location block list.
     */
    std::vector<ConfigLocationBlock>& location_block_list(void);

    /**
     * @brief Parses the given file stream and returns the parsed result as a string.
     * @param file_stream The input file stream to be parsed.
     * @return The parsed result as a string.
     */
    std::string parse(std::ifstream& file_stream);

    /**
     * @brief Prints the configuration of the server block.
     */
    void print_config(void) const;

  private:
    /**
     * @brief The server name.
     */
    std::string _server_name;

    /**
     * @brief The listen ip and port.
     */
    std::pair<std::string, std::string> _listen;

    /**
     * @brief The error log. (filename, log level)
     */
    std::vector<std::pair<std::string, weblog::LogLevel> > _error_log;

    /**
     * @brief The keepalive timeout. (in seconds)
     */
    unsigned int _keepalive_timeout;

    /**
     * @brief The location block list.
     */
    std::vector<ConfigLocationBlock> _location_block_list;

    /**
     * @brief Parses the configuration directive.
     * @param line The line to be parsed.
     */
    void _parse_config_directive(const std::string& line);

    /**
     * @brief Parses the server_name directive.
     * @param line The line to be parsed.
     * @return The server name.
     */
    std::pair<std::string, std::string>
    _parse_listen(const std::string& line, const std::string& directive);

    /**
     * @brief Parses the error_log directive.
     * @param line The line to be parsed.
     * @return The error log.
     */
    std::pair<std::string, weblog::LogLevel>
    _parse_error_log(const std::string& line, const std::string& directive);

    /**
     * @brief Parses the keepalive_timeout directive.
     * @param line The line to be parsed.
     * @return The keepalive timeout.
     */
    unsigned int _parse_keepalive_timeout(const std::string& line,
                                          const std::string& directive);
};

} // namespace webconfig
