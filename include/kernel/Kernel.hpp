/**
 * @file Kernel.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "Config.hpp"
#include "Reactor.hpp"
#include "Acceptor.hpp"
#include "defines.hpp"
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>

namespace webkernel
{

/**
 * @class Kernel
 * @brief The Kernel class is a concrete class for running the web server.
 */
class Kernel
{
  public:
    /**
     * @brief Constructor for Kernel with a specified configuration.
     * @param config The configuration to be set for the kernel.
     */
    Kernel(webconfig::Config& config);

    /**
     * @brief Copy constructor for Kernel.
     * @param other The Kernel object to be copied.
     */
    Kernel(const Kernel& other);

    /**
     * @brief Assignment operator for Kernel.
     * @param other The Kernel object to be assigned.
     * @return A reference to the assigned Kernel object.
     */
    Kernel& operator=(const Kernel& other);

    /**
     * @brief Destructor for Kernel.
     */
    ~Kernel();

    /**
     * @brief Runs the web server.
     */
    void run(void);

  private:
    Kernel();

    /**
     * @brief The configuration for the kernel.
     */
    webconfig::Config _config;

    /**
     * @brief The reactor for the kernel.
     */
    Reactor *_reactor;

    /**
     * @brief The acceptor for the kernel.
     */
    Acceptor *_acceptor;

    /**
     * @brief Initializes the kernel.
     */
    void _init(void);

    /**
     * @brief Initializes the listener.
     */
    void _init_listener(void);

    /**
     * @brief Creates a listen socket with the given IP address and port.
     * @param ip The IP address to be used for the listen socket.
     * @param port The port to be used for the listen socket.
     * @return The file descriptor of the listen socket.
     */
    int _create_listen_socket(const char* ip, const char* port);

    /**
     * @brief Creates the kernel subcomponents with the given configuration.
     * If the worker_processes is 1, the kernel will create a single reactor and a single worker structure.
     */
    void _create_single_reactor_single_worker(void);

    /**
     * @brief Creates the kernel subcomponents with the given configuration.
     * If the worker_processes is greater than 1, the kernel will create a single reactor and multiple worker structures.
     * with multiple worker processes. 
     */
    void _create_multi_reactor_multi_worker(void);
};

} // namespace webkernel
