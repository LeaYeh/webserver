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
#include "Singleton.hpp"
#include "defines.hpp"
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
    static Logger* create_instance();
    static Logger* create_instance(const std::string& filename);

    static void log(LogLevel level, const std::string& message);
    void set_level(LogLevel level);
    void set_file_mode(const std::string& filename);
    bool is_file_mode(void) const;
    LogLevel level(void) const;
    std::ofstream& file_stream(void);
    std::string get_level_str(LogLevel level) const;
    std::string get_color_level_str(LogLevel level) const;
    std::string get_current_time() const;

public:
    ~Logger();

private:
    std::ofstream _file_stream;
    bool _is_file_mode;
    LogLevel _level;

private:
    Logger();
    Logger(const std::string& filename);
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
};

} // namespace weblog
