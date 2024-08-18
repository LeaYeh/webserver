/**
 * @file Acceptor.hpp
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
#include "ConnectionHandler.hpp"
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

namespace webkernel
{

/**
 * @class Acceptor
 * @brief The Acceptor class is a concrete class for accepting connections.
 */
class Acceptor : public IHandler
{
  public:
    /**
     * @brief Default constructor for Acceptor.
     */
    Acceptor();

    /**
     * @brief Constructor for Acceptor with a specified reactor.
     * @param reactor The reactor to be set for the acceptor.
     */
    Acceptor(Reactor* reactor);

    /**
     * @brief Copy constructor for Acceptor.
     * @param other The Acceptor object to be copied.
     */
    Acceptor(const Acceptor& other);

    /**
     * @brief Assignment operator for Acceptor.
     * @param other The Acceptor object to be assigned.
     * @return A reference to the assigned Acceptor object.
     */
    Acceptor& operator=(const Acceptor& other);

    /**
     * @brief Destructor for Acceptor.
     */
    ~Acceptor();

    /**
     * @brief Creates a connection with the given IP address and port.
     * @param ip The IP address to be used for the connection.
     * @param port The port to be used for the connection.
     * @return The file descriptor of the connection.
     */
    int create_connection(const char* ip, const char* port);

    /**
     * @brief Handles the event of the given file descriptor.
     * @param fd The file descriptor to be handled.
     * @param events The events to be handled.
     */
    void handle_event(int fd, uint32_t events);

  private:
    /**
     * @brief The reactor of the acceptor.
     */
    Reactor* _reactor;
};

} // namespace webkernel
