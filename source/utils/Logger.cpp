/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:53 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/18 16:00:23 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Logger.hpp"

namespace weblog
{

Logger::Logger() : _is_file_mode(false), _level(INFO)
{
    std::cout << "Logger created: console mode" << std::endl;
}

Logger::Logger(const std::string& filename) : _is_file_mode(true), _level(INFO)
{
    _file_stream.open(filename.c_str(), std::ios::out | std::ios::app);
    if (!_file_stream.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        _is_file_mode = false;
    }
    else
        std::cout << "Logger created: file mode" << std::endl;
}

Logger::~Logger()
{
    if (_is_file_mode)
        _file_stream.close();
}

void Logger::log(LogLevel level, const std::string& message)
{
    if (level < _level)
        return;

    const int level_width = 50;
    const int message_width = 100;
    if (_is_file_mode)
    {
        _file_stream << _get_current_time() << " [" + _get_level_str(level) + "] "
                     << std::setw(level_width) << std::left
                     << std::setw(message_width) << std::left << message
                     << std::endl;
    }
    else
    {
        std::cout << _get_current_time() << " [" + _get_color_level_str(level) + "] "
                  << std::setw(level_width) << std::left
                  << std::setw(message_width) << std::left << message
                  << std::endl;
    }
}

void Logger::set_level(LogLevel level)
{
    std::cout << "Log level set to: " << _get_level_str(level) << std::endl;
    _level = level;
}

void Logger::set_file_mode(const std::string& filename)
{
    if (_is_file_mode)
        _file_stream.close();

    _file_stream.open(filename.c_str(), std::ios::out | std::ios::app);
    if (!_file_stream.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        _is_file_mode = false;
    }
    else
    {
        _is_file_mode = true;
        std::cout << "Logger switched to file mode" << std::endl;
    }
}

std::string Logger::_get_level_str(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

std::string Logger::_get_color_level_str(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "\033[34mDEBUG\033[0m";
    case INFO:
        return "\033[32mINFO\033[0m";
    case WARNING:
        return "\033[33mWARNING\033[0m";
    case ERROR:
        return "\033[31mERROR\033[0m";
    case CRITICAL:
        return "\033[41mCRITICAL\033[0m";
    default:
        return "\033[37mUNKNOWN\033[0m";
    }
}

std::string Logger::_get_current_time()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

Logger logger;

} // namespace weblog
