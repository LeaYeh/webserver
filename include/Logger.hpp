/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:34 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/07 20:55:58 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "defines.hpp"
#include <ctime>
#include <errno.h> // errno, strerror
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <unistd.h> // pipe
// #include <sys/stat.h>
// #include <sys/types.h>

namespace weblog
{

/**
 * @brief Log levels for the Logger.
 *
 * Defines the severity of log messages.
 */
enum LogLevel
{
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Logger class for logging messages with different severity levels.
 *
 * The Logger class provides functionality to log messages to either the console
 * or a file with different log levels.
 */
class Logger
{
  public:
    static Logger& get_instance(void);
    static Logger& get_instance(const std::string& filename);

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
    std::string _log_file;
    bool _is_file_mode;
    LogLevel _level;
    int _pipe_fd[2];

    Logger();
    Logger(const std::string& filename);
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);

    void _start_log_process(void);
    void _log_writer(void);
    static void _self_cleanup(void);

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
