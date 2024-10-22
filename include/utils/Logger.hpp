/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:34 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/18 15:52:41 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "defines.hpp"
#include <ctime>
#include <fstream>
#include <iomanip>
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
    ~Logger();

    void log(LogLevel level, const std::string& message);
    void setLevel(LogLevel level);
    void setFileMode(const std::string& filename);

  protected:
  private:
    Logger();
    Logger(const std::string& filename);
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);

    std::ofstream _file_stream;
    bool _is_file_mode;
    LogLevel _level;

    std::string _getLevelStr(LogLevel level);
    ;
    std::string _getColorLevelStr(LogLevel level);
    ;
    std::string _getCurrentTime();
};

} // namespace weblog
