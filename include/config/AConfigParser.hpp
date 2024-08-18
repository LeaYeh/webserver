/**
 * @file AConfigParser.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "utils/config_utils.hpp"
#include "defines.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace webconfig
{

/**
 * @class AConfigParser
 * @brief The AConfigParser class is an abstract base class for parsing configuration files.
 */
class AConfigParser
{
  public:
    /**
     * @brief Default constructor for AConfigParser.
     */
    AConfigParser();

    /**
     * @brief Constructor for AConfigParser with a specified block level.
     * @param block_level The block level to be set for the parser.
     */
    AConfigParser(ConfigBlockLevel block_level);

    /**
     * @brief Copy constructor for AConfigParser.
     * @param other The AConfigParser object to be copied.
     */
    AConfigParser(const AConfigParser& other);

    /**
     * @brief Assignment operator for AConfigParser.
     * @param other The AConfigParser object to be assigned.
     * @return A reference to the assigned AConfigParser object.
     */
    AConfigParser& operator=(const AConfigParser& other);

    /**
     * @brief Virtual destructor for AConfigParser.
     */
    virtual ~AConfigParser();

    /**
     * @brief Parses the given file stream and returns the parsed result as a string.
     * @param file_stream The input file stream to be parsed.
     * @return The parsed result as a string.
     */
    virtual std::string parse(std::ifstream& file_stream) = 0;

    /**
     * @brief Prints the configuration of the parser.
     */
    virtual void print_config(void) const = 0;

  protected:
    /**
     * @brief The block level of the parser.
     */
    ConfigBlockLevel _block_level;

    /**
     * @brief The set of valid directives for the parser.
     */
    std::set<std::string> _valid_directives;

    /**
     * @brief Checks if the given line needs to be skipped during parsing.
     * @param line The line to be checked.
     * @return True if the line needs to be skipped, false otherwise.
     */
    bool _need_to_skip(const std::string& line) const;

    /**
     * @brief Checks if the given line is a valid sentence.
     * @param line The line to be checked.
     * @return True if the line is a valid sentence, false otherwise.
     */
    bool _is_valid_sentence(const std::string& line) const;

    /**
     * @brief Checks if the given line is a scope symbol.
     * @param line The line to be checked.
     * @return True if the line is a scope symbol, false otherwise.
     */
    bool _is_scope_symbol(const std::string& line) const;

    /**
     * @brief Gets the directive name from the given line.
     * @param line The line to extract the directive name from.
     * @return The directive name.
     */
    std::string _get_directive_name(const std::string& line) const;

    /**
     * @brief Checks if the given directive is valid.
     * @param directive The directive to be checked.
     * @return True if the directive is valid, false otherwise.
     */
    bool _is_valid_directive(const std::string& directive) const;

    /**
     * @brief Parses the configuration directive from the given line.
     * @param line The line to be parsed.
     */
    virtual void _parse_config_directive(const std::string& line) = 0;

  private:
};

} // namespace webconfig
