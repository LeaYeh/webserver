/**
 * @file ConfigLocationBlock.hpp
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
#include <string>
#include <vector>

namespace webconfig
{

/**
 * @class ConfigLocationBlock
 * @brief The ConfigLocationBlock class is a concrete class for parsing the location block of the configuration file.
 */
class ConfigLocationBlock : public AConfigParser
{
  public:
    /**
     * @brief Default constructor for ConfigLocationBlock.
     */
    ConfigLocationBlock();

    /**
     * @brief Copy constructor for ConfigLocationBlock.
     * @param other The ConfigLocationBlock object to be copied.
     */
    ConfigLocationBlock(const ConfigLocationBlock& other);

    /**
     * @brief Assignment operator for ConfigLocationBlock.
     * @param other The ConfigLocationBlock object to be assigned.
     * @return A reference to the assigned ConfigLocationBlock object.
     */
    ConfigLocationBlock& operator=(const ConfigLocationBlock& other);

    /**
     * @brief Destructor for ConfigLocationBlock.
     */
    ~ConfigLocationBlock();


    /**
     * @brief Parses the given file stream and returns the parsed result as a string.
     * @param file_stream The input file stream to be parsed.
     * @return The parsed result as a string.
     */
    std::string parse(std::ifstream& file_stream);

    /**
     * @brief Prints the configuration of the location block.
     */
    void print_config(void) const;

  private:
    /**
     * @brief The route of the location block.
     */
    std::string _route;

    /**
     * @brief The limit_except of the location block.
     */
    std::vector<webshell::RequestMethod> _limit_except;

    /**
     * @brief The root of the location block.
     */
    std::string _root;

    /**
     * @brief The index of the location block.
     */
    std::string _index;

    /**
     * @brief The error page of the location block.
     */
    std::string _redirect;

    /**
     * @brief The autoindex of the location block.
     */
    bool _autoindex;

    /**
     * @brief The cgi_extension of the location block.
     */
    std::string _cgi_extension;

    /**
     * @brief The cgi_path of the location block.
     */
    std::string _cgi_path;

    /**
     * @brief The enable_upload of the location block.
     */
    bool _enable_upload;

    /**
     * @brief The upload_path of the location block.
     */
    std::string _upload_path;

    /**
     * @brief Parses the configuration directive.
     * @param line The line to be parsed.
     */
    void _parse_config_directive(const std::string& line);

    /**
     * @brief Parses the route directive.
     * @param line The line to be parsed.
     * @return The route of the location block.
     */
    std::vector<webshell::RequestMethod> _parse_limit_except(const std::string& line, const std::string& directive);

    /**
     * @brief Parses the root directive.
     * @param line The line to be parsed.
     * @return The root of the location block.
     */
    bool _parse_autoindex(const std::string& line, const std::string& directive);

    /**
     * @brief Parses the index directive.
     * @param line The line to be parsed.
     * @return The index of the location block.
     */
    bool _parse_enable_upload(const std::string& line, const std::string& directive);

};

} // namespace webconfig
