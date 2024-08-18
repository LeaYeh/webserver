/**
 * @file Config.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "ConfigGlobalBlock.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigServerBlock.hpp"
#include "defines.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace webconfig
{

/**
 * @class Config
 * @brief The Config class is a concrete class for parsing configuration files.
 */
class Config
{
  public:
    /**
     * @brief Default constructor for Config.
     */
    Config();

    /**
     * @brief Constructor for Config with a specified filename.
     * @param filename The filename to be set for the parser.
     */
    Config(const std::string& filename);

    /**
     * @brief Copy constructor for Config.
     * @param other The Config object to be copied.
     */
    Config(const Config& other);

    /**
     * @brief Assignment operator for Config.
     * @param other The Config object to be assigned.
     * @return A reference to the assigned Config object.
     */
    Config& operator=(const Config& other);

    /**
     * @brief Destructor for Config.
     */
    ~Config();

    /**
     * @brief Parses the configuration file.
     */
    void parse(void);

    /**
     * @brief Prints the configuration of the parser.
     */
    void print_config(void) const;

    /**
     * @brief Returns the filename of the parser.
     * @return The filename of the parser.
     */
    std::string filename(void) const;

    /**
     * @brief Returns the block level of the parser.
     * @return The block level of the parser.
     */
    ConfigGlobalBlock& global_block(void);

    /**
     * @brief Returns the block level of the parser.
     * @return The block level of the parser.
     */
    ConfigHttpBlock& http_block(void);

    /**
     * @brief Returns the block level of the parser.
     * @return The block level of the parser.
     */
    std::vector<ConfigServerBlock>& server_block_list(void);

  private:
    /**
     * @brief The block level of the parser.
     */
    ConfigBlockLevel _current_block_level;

    /**
     * @brief The filename of the parser.
     */
    std::string _filename;

    /**
     * @brief The input file stream of the parser.
     */
    std::ifstream _file_stream;

    /**
     * @brief The global block of the parser.
     */
    ConfigGlobalBlock _global_block;

    /**
     * @brief The HTTP block of the parser.
     */
    ConfigHttpBlock _http_block;

    /**
     * @brief The list of server blocks of the parser.
     */
    std::vector<ConfigServerBlock> _server_block_list;

    /**
     * @brief Checks if the given line needs to be skipped during parsing.
     * @param line The line to be checked.
     * @return True if the line needs to be skipped, false otherwise.
     */
    bool _set_block_level(const std::string& line);
};

} // namespace webconfig
