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
#include "Singleton.hpp"
#include <ctime>
#include <fstream>

namespace weblog
{

/**
 * @brief Logger class for logging messages with different severity levels.
 *
 * The Logger class provides functionality to log messages to either the console
 * or a file with different log levels.
 */
class Logger : public templates::Singleton<Logger, std::string>
{
  public:
    ~Logger();

    static Logger* createInstance();
    static Logger* createInstance(const std::string& filename);

    static void log(LogLevel level, const std::string& message);
    void setLevel(LogLevel level);
    void setFileMode(const std::string& filename);
    bool isFileMode(void) const;
    LogLevel level(void) const;
    std::ofstream& fileStream(void);
    std::string getLevelStr(LogLevel level) const;
    std::string getColorLevelStr(LogLevel level) const;
    std::string getCurrentTime() const;

  protected:
  private:
    Logger();
    Logger(const std::string& filename);
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);

    std::ofstream _file_stream;
    bool _is_file_mode;
    LogLevel _level;
};

} // namespace weblog
