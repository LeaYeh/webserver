/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:53 by lyeh              #+#    #+#             */
/*   Updated: 2024/07/29 22:50:25 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

namespace weblog
{

Logger::Logger() : _isFileMode(false), _level(INFO)
{
    std::cout << "Logger created: console mode" << std::endl;
}

Logger::Logger(const std::string& filename) : _isFileMode(true), _level(INFO)
{
    _fileStream.open(filename.c_str(), std::ios::out | std::ios::app);
    if (!_fileStream.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        _isFileMode = false;
    }
    else
        std::cout << "Logger created: file mode" << std::endl;
}

Logger::~Logger()
{
    if (_isFileMode)
        _fileStream.close();
}

void Logger::log(LogLevel level, const std::string& message)
{
    if (level < _level)
        return;

    std::string logMessage =
        _getCurrentTime() + " [" + _getLevelStr(level) + "] " + message + "\n";

    if (_isFileMode)
        _fileStream << logMessage;
    else
        std::cout << logMessage;
}

void Logger::setLevel(LogLevel level)
{
    std::cout << "Log level set to: " << _getLevelStr(level) << std::endl;
    _level = level;
}

std::string Logger::_getLevelStr(LogLevel level)
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

std::string Logger::_getCurrentTime()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

} // namespace weblog
