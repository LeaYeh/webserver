/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lyeh <lyeh@student.42vienna.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 22:49:41 by lyeh              #+#    #+#             */
/*   Updated: 2024/08/15 18:26:11 by lyeh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "utils/Logger.hpp"
#include "defines.hpp"

void uncatchable_exception_handler(void)
{
    std::cerr << "An uncatchable exception has occurred. Exiting..."
              << std::endl;
    exit(FAILURE);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return (FAILURE);
    }
    try
    {
        webconfig::Config config(argv[1]);
        config.parse();
        config.print_config();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (FAILURE);
    }
    std::set_terminate(uncatchable_exception_handler);

    return (SUCCESS);
}
