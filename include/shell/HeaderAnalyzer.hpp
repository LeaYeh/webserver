#pragma once

#include "defines.hpp"
#include <iostream>
#include <map>
#include <vector>

namespace webshell
{

// TODO: Fix the naming convention to be more consistent.
//  All custom functions and variables should be named with snake_case.
//  All class and enum names should be named with CamelCase.
class HeaderAnalyzer
{
  public:
    HeaderAnalyzer();
    HeaderAnalyzer(const HeaderAnalyzer& other);
    HeaderAnalyzer& operator=(const HeaderAnalyzer& other);
    ~HeaderAnalyzer();

    void parse_headers(const std::string& line,
                       RequestHeaderState current_state);

    std::string host() const;
    std::string accept() const;
    std::string accept_encoding() const;
    std::string connection() const;
    ConnectionType connection_type() const;
    std::string content_length() const;

  private:
    // TODO: Seperate the variables name with prefix by different usage.
    RequestHeaderState _header_state;
    std::string _host;
    std::string _header_new_line;
    std::string _host_name;
    std::string _accept;
    std::string _accept_type;
    std::string _accept_encoding;
    std::string _accept_encoding_type;
    std::string _connection;
    ConnectionType _connection_type;
    std::string _content_type;
    std::string _content_type_name;
    std::string _content_length;
    std::string _content_length_nbr;
    std::string _invalid_header;
    std::string _the_rest;
};

} // namespace webshell
