#pragma once

#include "defines.hpp"
#include <iostream>
#include <map>
#include <vector>

namespace webshell
{

/**
 * @brief Abstract class for analyzing HTTP headers and extracting information.
 *
 * The AHeaderAnalyzer class provides an interface for analyzing HTTP headers.
 * It defines the analyze function that must be implemented by derived classes.
 */
class HeaderAnalyzer
{
  public:
    HeaderAnalyzer();
    HeaderAnalyzer(const HeaderAnalyzer& src);
    HeaderAnalyzer& operator=(const HeaderAnalyzer& src);
    ~HeaderAnalyzer();

    void parse_headers(const std::string& line,
                       RequestHeaderState currentState);

    std::string host() const;
    std::string accept() const;
    std::string accept_encoding() const;
    std::string connection() const;
    std::string content_type() const;
    std::string content_length() const;

  private:
  
    RequestHeaderState _header_state;
    std::string _host;
    std::string _header_new_line;
    std::string _host_name;
    std::string _accept;
    std::string _accept_type;
    std::string _accept_encoding;
    std::string _accept_encoding_type;
    std::string _connection;
    std::string _connection_type;
    std::string _content_type;
    std::string _content_type_name;
    std::string _content_length;
    std::string _content_length_nbr;
    std::string _invalid_header;
    std::string _the_rest;
};

} // namespace webshell
