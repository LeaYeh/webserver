/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:34 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/13 20:39:04 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "defines.hpp"
#include <ctime>
#include <fstream>
#include <iostream>

namespace weblog
{

/**
 * @brief Logger class for logging messages with different severity levels.
 *
 * The Logger class provides functionality to log messages to either the console
 * or a file with different log levels.
 */
class Logger
{
  public:
    /**
     * @brief Construct a new Logger object.
     *
     * Initializes the Logger to log messages to the console.
     * The log level is set to INFO by default.
     */
    Logger();

    /**
     * @brief Construct a new Logger object with a file.
     *
     * Initializes the Logger to log messages to the specified file.
     *
     * @param filename The name of the file to log messages to.
     */
    Logger(const std::string& filename);

    /**
     * @brief Destroy the Logger object.
     *
     * Closes the file stream if it is open.
     */
    ~Logger();

    /**
     * @brief Log a message with the specified log level.
     *
     * Logs a message with the given severity level to the console or file.
     *
     * @param level The severity level of the log message.
     * @param message The message to log.
     */
    void log(LogLevel level, const std::string& message);

    /**
     * @brief Set the minimum log level for the Logger.
     *
     * Sets the minimum severity level for messages to be logged. Messages with
     * a severity level below this will be ignored.
     *
     * @param level The minimum log level.
     */
    void set_level(LogLevel level);

  protected:
  private:
    Logger(const Logger& other); ///< Copy constructor (deleted).
    Logger&
    operator=(const Logger& other); ///< Copy assignment operator (deleted).

    std::ofstream _file_stream; ///< File stream for logging to a file.
    bool _is_file_mode; ///< Flag indicating if the Logger is in file mode.
    LogLevel _level;  ///< Minimum log level for messages.

    /**
     * @brief Get the string representation of a log level.
     *
     * Converts a LogLevel enum value to its corresponding string
     * representation.
     *
     * @param level The log level to convert.
     * @return The string representation of the log level.
     */
    std::string _get_level_str(LogLevel level);

    /**
     * @brief Get the current time as a string.
     *
     * Retrieves the current system time and formats it as a string.
     *
     * @return The current time as a string.
     */
    std::string _get_current_time();
};

} // namespace weblog
