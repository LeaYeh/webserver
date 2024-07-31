#pragma once
#include <cstddef>
#include <string>

namespace webserver
{

/**
 * @brief Content types for the HTTP headers.
 *
 * Defines the different content types for the HTTP headers.
 */
enum ContentType
{
    TEXT,
    IMAGE,
    AUDIO,
    VIDEO,
    APPLICATION,
    MULTIPART,
    MESSAGE,
    MODEL,
    OTHER
};

/**
 * @brief Connection types for the HTTP headers.
 *
 * Defines the different connection types for the HTTP headers.
 */
enum Connection
{
    KEEP_ALIVE,
    CLOSE
};

/**
 * @brief Encoding types for the HTTP headers.
 *
 * Defines the different encoding types for the HTTP headers.
 */
enum Encoding
{
    GZIP,
    DEFLATE,
    BR,
    IDENTITY,
    CHUNKED,
    COMPRESS
};

/**
 * @brief HTTP methods for the HTTP headers.
 *
 * Defines the different HTTP methods for the HTTP headers.
 */
enum Method
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE
};

/**
 * @brief HTTP versions for the HTTP headers.
 *
 * Defines the different HTTP versions for the HTTP headers.
 */
enum Version
{
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0
};

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
     * the version, method, host, content length, content type, connection, encoding,
     * cookie, and cache control.
     */
    virtual void analyze(void) = 0;

  protected:
    Version _version;
    Method _method;
    std::string _host;
    size_t _content_length;
    ContentType _content_type;
    Connection _connection;
    Encoding _encoding;
    std::string _cookie;
    std::string _cache_control;
};

} // namespace webserver
