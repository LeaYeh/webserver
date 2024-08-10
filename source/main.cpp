/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:41 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/10 17:51:24 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "defines.hpp"
#include "ServerConfig.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return (FAILURE);
    }
    try
    {
        webconfig::ServerConfig server_config(argv[1]);
        server_config.parse();
        std::cout << "worker_processes: " << server_config.worker_processes() << std::endl;
        std::cout << "worker_connections: " << server_config.worker_connections() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }

    return (SUCCESS);
}
