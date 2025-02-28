/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:41 by lyeh              #+#    #+#             */
/*   Updated: 2025/02/23 20:22:52 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ExecuteWithUnwind.hpp"
#include "Kernel.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>

volatile sig_atomic_t stop_flag = 0;
extern char **environ; 

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
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return (FAILURE);
    }
    weblog::Logger::instantiate();
    webconfig::Config* config = NULL;
    try {
        webconfig::Config::instantiate(argv[1]);
        config = webconfig::Config::instance();
        signal(SIGINT, handle_terminate_signal);
        weblog::Logger* logger = weblog::Logger::instance();
        logger->set_level(weblog::DEBUG);
        // logger->set_file_mode("webserver.log");

        config->print_config();
        webkernel::Kernel kernel;

        kernel.run();
    }
    // catch (const std::exception& e) {
    //     // ExecuteWithUnwind: unwind whole webserv then execute the external process
    //     // ReturnWithUnwind: unwind whole webserv then return on main
    //     if (execute_cgi) {}
    //     else if (i) {}
        
    //     config->destroy();
    //     weblog::Logger::destroy();
    // }
    catch (ExecuteWithUnwind& e)
    {
        // char* argv[2];
        // argv[0] = strdup("loop.sh");
        // argv[1] = NULL;
        config->destroy();
        weblog::Logger::destroy();
        e.execute();
        // execve(lol, argv, environ);
        return (FAILURE);
    }
    catch (int i)
    {
        (void)i;
        config->destroy();
        weblog::Logger::destroy();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        config->destroy();
        weblog::Logger::destroy();
        return (FAILURE);
    }
    catch (...) {
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
