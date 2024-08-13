/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:53 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/07 22:05:02 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <stdexcept>
#include <unistd.h>

namespace weblog
{

Logger& Logger::get_instance(void)
{
    static Logger instance;

    return instance;
}

Logger& Logger::get_instance(const std::string& filename)
{
    static Logger instance(filename);

    return instance;
}

Logger::Logger() : _is_file_mode(false), _level(INFO)
{
    std::cout << "Logger created: console mode" << std::endl;
    if (pipe(_pipe_fd) == -1)
        throw std::runtime_error("Error creating pipe");
    _start_log_process();
}

Logger::Logger(const std::string& filename)
    : _log_file(filename), _is_file_mode(true), _level(INFO)
{
    std::cout << "Logger created: file mode" << std::endl;
    if (pipe(_pipe_fd) == -1)
        throw std::runtime_error("Error creating pipe");
    _start_log_process();
}

Logger::~Logger()
{
    std::cout << "haha2\n";
    utils::safe_close_pipe(_pipe_fd);
}

void Logger::log(LogLevel level, const std::string& message)
{
    if (level < _level)
        return;

    std::string logMessage = _get_current_time() + " [" +
                             _get_level_str(level) + "] " + message + "\n";
    write(_pipe_fd[1], logMessage.c_str(), logMessage.size());
}

void Logger::set_level(LogLevel level)
{
    _level = level;
}

void Logger::_log_writer(void)
{
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(_pipe_fd[0], buffer, sizeof(buffer))) > 0)
    {
        if (!_is_file_mode)
            write(STDOUT_FILENO, buffer, bytes_read);
        else
        {
            int fd = open(_log_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            write(fd, buffer, bytes_read);
            close(fd);
        }
    }
    std::atexit(_self_cleanup);
}

void Logger::_self_cleanup(void)
{
    std::cout << "haha1\n";
    Logger& logger = Logger::get_instance();
    utils::safe_close_pipe(logger._pipe_fd);
}

void Logger::_start_log_process(void)
{
    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("Error forking process");
    else if (pid == 0)
    {
        close(_pipe_fd[1]);
        _log_writer();
    }
    else
        close(_pipe_fd[0]);
}

std::string Logger::_get_level_str(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return (STY_BLU"DEBUG"STY_RES);
    case INFO:
        return (STY_GRE"INFO"STY_RES);
    case WARNING:
        return (STY_YEL"WARNING"STY_RES);
    case ERROR:
        return (STY_RED"ERROR"STY_RES);
    case CRITICAL:
        return (STY_RED"CRITICAL"STY_RES);
    default:
        return (STY_GRA_BG"UNKNOWN"STY_RES);
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

} // namespace weblog
