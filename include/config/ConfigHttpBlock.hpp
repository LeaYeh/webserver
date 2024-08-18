/**
 * @file ConfigHttpBlock.hpp
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
#include "defines.hpp"
#include <set>
#include <vector>

namespace webconfig
{

/**
 * @class ConfigHttpBlock
 * @brief The ConfigHttpBlock class is a concrete class for parsing the http block of the configuration file.
 */
class ConfigHttpBlock : public AConfigParser
{
  public:
    /**
     * @brief Default constructor for ConfigHttpBlock.
     */
    ConfigHttpBlock();

    /**
     * @brief Copy constructor for ConfigHttpBlock.
     * @param other The ConfigHttpBlock object to be copied.
     */
    ConfigHttpBlock(const ConfigHttpBlock& other);

    /**
     * @brief Assignment operator for ConfigHttpBlock.
     * @param other The ConfigHttpBlock object to be assigned.
     * @return A reference to the assigned ConfigHttpBlock object.
     */
    ConfigHttpBlock& operator=(const ConfigHttpBlock& other);

    /**
     * @brief Destructor for ConfigHttpBlock.
     */
    ~ConfigHttpBlock();

    /**
     * @brief Parses the given file stream and returns the parsed result as a string.
     * @param file_stream The input file stream to be parsed.
     * @return The parsed result as a string.
     */
    std::string parse(std::ifstream& file_stream);

    /**
     * @brief Prints the configuration of the http block.
     */
    void print_config(void) const;

  private:
    /**
      * @brief The maximum body size of the client.
     */
    unsigned int _client_max_body_size;

    /**
     * @brief The default content type.
     */
    webshell::ContentType _default_type;

    /**
     * @brief The list of error pages.
     */
    std::vector<std::pair<webshell::StatusCode, std::string> > _error_page_list;

    /**
     * @brief Parses the configuration directive.
     * @param line The line to be parsed.
     */
    void _parse_config_directive(const std::string& line);

    /**
     * @brief Parses the client_max_body_size directive.
     * @param line The line to be parsed.
     * @return The maximum body size of the client.
     */
    unsigned int _parse_client_max_body_size(const std::string& line);

    /**
     * @brief Parses the default_type directive.
     * @param line The line to be parsed.
     * @return The default content type.
     */
    webshell::ContentType _parse_default_type(const std::string& line);

    /**
     * @brief Parses the error_page directive.
     * @param line The line to be parsed.
     * @return The error page.
     */
    std::pair<webshell::StatusCode, std::string> _parse_error_page(const std::string& line);
};

} // namespace webconfig
