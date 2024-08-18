/**
 * @file Reactor.hpp
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
#include "defines.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

namespace webkernel
{

class Acceptor;

/**
  * @class Reactor
  * @brief The Reactor class is a concrete class for handling events.
  */
class Reactor
{
  public:
    /**
      * @brief Default constructor for Reactor.
      */
    Reactor();

    /**
      * @brief Constructor for Reactor with a specified reactor type.
      * @param type The reactor type to be set for the reactor.
      */
    Reactor(ReactorType type);

    /**
      * @brief Copy constructor for Reactor.
      * @param other The Reactor object to be copied.
      */
    Reactor(const Reactor&);

    /**
      * @brief Assignment operator for Reactor.
      * @param other The Reactor object to be assigned.
      * @return A reference to the assigned Reactor object.
      */
    Reactor& operator=(const Reactor&);

    /**
      * @brief Destructor for Reactor.
      */
    ~Reactor();

    /**
      * @brief Runs the reactor.
      */
    void run(void);

    /**
      * @brief Registers a handler with the given file descriptor and events.
      * @param fd The file descriptor to be used for the handler.
      * @param handler The handler to be registered.
      * @param events The events to be registered.
      */
    void register_handler(int fd, IHandler* handler, uint32_t events);

    /**
      * @brief Modifies the events of the given file descriptor.
      * @param fd The file descriptor to be modified.
      * @param events The events to be modified.
      */
    void remove_handler(int fd);

  private:
    /**
      * @brief The reactor type of the reactor.
      * @details REACTOR, DISPATCHER, or WORKER
      */
    ReactorType _type;

    /**
      * @brief The file descriptor of the epoll.
      */
    int _epoll_fd;

    /**
      * @brief Records the registered handlers with the file descriptor.
      */
    std::map<int, IHandler*> _handlers;

    /**
      * @brief Initializes the epoll.
      */
    void _init_epoll(void);
};

} // namespace webkernel
