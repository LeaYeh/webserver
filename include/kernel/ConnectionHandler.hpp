/**
 * @file ConnectionHandler.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "IHandler.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include <errno.h>
#include <iostream>
#include <string.h>

namespace webkernel
{

/**
 * @class ConnectionHandler
 * @brief The ConnectionHandler class is a concrete class for handling connections.
 */
class ConnectionHandler : public IHandler
{
  public:
    /**
     * @brief Default constructor for ConnectionHandler.
     */
    ConnectionHandler();

    /**
     * @brief Constructor for ConnectionHandler with a specified connection file descriptor and reactor.
     * @param conn_fd The connection file descriptor to be set for the connection handler.
     * @param reactor The reactor to be set for the connection handler.
     */
    ConnectionHandler(int conn_fd, Reactor* reactor);

    /**
     * @brief Copy constructor for ConnectionHandler.
     * @param other The ConnectionHandler object to be copied.
     */
    ConnectionHandler(const ConnectionHandler& other);

    /**
     * @brief Assignment operator for ConnectionHandler.
     * @param other The ConnectionHandler object to be assigned.
     * @return A reference to the assigned ConnectionHandler object.
     */
    ConnectionHandler& operator=(const ConnectionHandler& other);

    /**
     * @brief Destructor for ConnectionHandler.
     */
    ~ConnectionHandler();

    /**
     * @brief Handles the event about the connection. (e.g., read, write, etc.)
     *
     * @param fd
     * @param events
     */
    void handle_event(int fd, uint32_t events);

  private:
    int _conn_fd;
    Reactor* _reactor;
};

} // namespace webkernel
