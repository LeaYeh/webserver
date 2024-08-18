/**
 * @file ConfigGlobalBlock.hpp
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
#include <set>
#include <string>

namespace webconfig
{

/**
 * @class ConfigGlobalBlock
 * @brief The ConfigGlobalBlock class is a concrete class for parsing the global block of the configuration file.
 */
class ConfigGlobalBlock : public AConfigParser
{
  public:

    /**
     * @brief Default constructor for ConfigGlobalBlock.
     */
    ConfigGlobalBlock();

    /**
     * @brief Copy constructor for ConfigGlobalBlock.
     * @param other The ConfigGlobalBlock object to be copied.
     */
    ConfigGlobalBlock(const ConfigGlobalBlock& other);

    /**
     * @brief Assignment operator for ConfigGlobalBlock.
     * @param other The ConfigGlobalBlock object to be assigned.
     * @return A reference to the assigned ConfigGlobalBlock object.
     */
    ConfigGlobalBlock& operator=(const ConfigGlobalBlock& other);

    /**
     * @brief Destructor for ConfigGlobalBlock.
     */
    ~ConfigGlobalBlock();

    /**
     * @brief Prints the configuration of the global block.
     */
    void print_config(void) const;

    /**
     * @brief Parses the given file stream and returns the parsed result as a string.
     * @param file_stream The input file stream to be parsed.
     * @return The parsed result as a string.
     */
    std::string parse(std::ifstream &file_stream);

    /**
     * @brief Gets the number of worker processes.
     * @return The number of worker processes.
     */
    unsigned int worker_processes(void) const;

    /**
     * @brief Gets the number of worker connections.
     * @return The number of worker connections.
     */
    unsigned int worker_connections(void) const;

  private:

    /**
     * @brief Parses the configuration directive.
     */
    unsigned int _worker_processes;

    /**
     * @brief The number of worker connections.
     */
    unsigned int _worker_connections;

    /**
     * @brief Parses the configuration directive.
     * @param line The line to be parsed.
     */
    void _parse_config_directive(const std::string& line);
};

} // namespace webconfig
