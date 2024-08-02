#pragma once
#include <string>

namespace webserver
{

/**
 * @brief Parser states for the request parser.
 *
 * Defines the different states the request parser can be in.
 */
enum ParserState
{
    REQUEST_LINE,
    HEADER,
    BODY,
    FINISHED
};

/**
 * @brief Abstract class for parsing HTTP requests.
  *
  * The ARequestParser class provides an interface for parsing HTTP requests.
  * It defines the parse function that must be implemented by derived classes.
  * Use state machine to parse the raw request.
  */
class ARequestParser
{
  public:
    /**
     * @brief Destroy the ARequestParser object.
     */
    virtual ~ARequestParser()
    {
    }

    /**
     * @brief Parse the raw request.
     *
     * This function parses the raw request and extracts the request line,
     * header, and body.
     *
     * @param raw_request The raw request to parse.
     */
    virtual void parse(const std::string& raw_request) = 0;
    std::string request_line() const;
    std::string header() const;
    std::string body() const;

  protected:
    ParserState _state;
    std::string _request_line;
    std::string _header;
    std::string _body;
};

} // namespace webserver
