/**
 * @file AHeaderAnalyzer.hpp
 * @author Lea Yeh (lea.yeh.ml@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "defines.hpp"
#include <cstddef>
#include <string>

namespace webshell
{

/**
 * @brief Abstract class for analyzing HTTP headers and extracting information.
 *
 * The AHedaerAnalyzer class provides an interface for analyzing HTTP headers.
 * It defines the analyze function that must be implemented by derived classes.
 */
class AHedaerAnalyzer
{
  public:
    virtual ~AHedaerAnalyzer()
    {
    }

    /**
     * @brief Analyze the HTTP headers.
     *
     * This function analyzes the HTTP headers and extracts information such as
     * the version, method, host, content length, content type, connection,
     * encoding, cookie, and cache control.
     */
    virtual void analyze(void) = 0;

  protected:
    HttpVersion _version;
    RequestMethod _method;
    std::string _host;
    size_t _content_length;
    ContentType _content_type;
    ConnectionType _connection;
    Encoding _encoding;
    std::string _cookie;
    std::string _cache_control;
};

} // namespace webshell
