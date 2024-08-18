/**
 * @file IHandler.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

namespace webkernel
{

/**
 * @class IHandler
 * @brief The IHandler class is an interface class for handling events.
 */
class IHandler
{
  public:
    /**
     * @brief Destructor for IHandler.
     */
    virtual ~IHandler()
    {
    }

    /**
     * @brief Handles the event about the file descriptor and the events.
     * @param fd The file descriptor to be handled.
     * @param events The events to be handled.
     */
    virtual void handle_event(int fd, uint32_t events) = 0;
};

} // namespace webkernel
