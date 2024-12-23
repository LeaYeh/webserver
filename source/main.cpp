/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:41 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/18 16:31:15 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Kernel.hpp"
#include "defines.hpp"
#include "utils/Logger.hpp"
#include <csignal>
#include <cstdlib>

volatile sig_atomic_t stop_flag = 0;

void handle_terminate_signal(int signum)
{
    stop_flag = signum;
}

void uncatchable_exception_handler(void)
{
    std::cerr << "An uncatchable exception has occurred. Exiting..."
              << std::endl;
    exit(FAILURE);
}

int main(int argc, char** argv)
{
    std::set_terminate(uncatchable_exception_handler);
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return (FAILURE);
    }
    weblog::Logger::instantiate();
    webconfig::Config::instantiate(argv[1]);
    webconfig::Config* config = webconfig::Config::instance();
    try
    {
        signal(SIGINT, handle_terminate_signal);
        weblog::Logger* logger = weblog::Logger::instance();
        logger->setLevel(weblog::DEBUG);
        // weblog::logger->setFileMode("webserver.log");

        config->printConfig();
        webkernel::Kernel kernel;

        kernel.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        config->destroy();
        weblog::Logger::destroy();
        return (FAILURE);
    }
    catch (...)
    {
        std::cerr << "An unknown exception has occurred. Exiting..."
                  << std::endl;
        config->destroy();
        weblog::Logger::destroy();
        return (FAILURE);
    }
    config->destroy();
    weblog::Logger::destroy();
    return (SUCCESS);
}
