#include "HeaderAnalyzer.hpp"

namespace webshell
{

HeaderAnalyzer::HeaderAnalyzer() : _connection_type(KEEP_ALIVE)
{
}

HeaderAnalyzer::HeaderAnalyzer(const HeaderAnalyzer& other)
    : _header_state(other._header_state), _host(other._host),
      _header_new_line(other._header_new_line), _host_name(other._host_name),
      _accept(other._accept), _accept_type(other._accept_type),
      _accept_encoding(other._accept_encoding),
      _accept_encoding_type(other._accept_encoding_type),
      _connection(other._connection), _connection_type(other._connection_type),
      _content_type(other._content_type),
      _content_type_name(other._content_type_name),
      _content_length(other._content_length),
      _content_length_nbr(other._content_length_nbr),
      _invalid_header(other._invalid_header), _the_rest(other._the_rest)
{
}

HeaderAnalyzer& HeaderAnalyzer::operator=(const HeaderAnalyzer& other)
{
    if (this != &other)
    {
        _header_state = other._header_state;
        _host = other._host;
        _header_new_line = other._header_new_line;
        _host_name = other._host_name;
        _accept = other._accept;
        _accept_type = other._accept_type;
        _accept_encoding = other._accept_encoding;
        _accept_encoding_type = other._accept_encoding_type;
        _connection = other._connection;
        _connection_type = other._connection_type;
        _content_type = other._content_type;
        _content_type_name = other._content_type_name;
        _content_length = other._content_length;
        _content_length_nbr = other._content_length_nbr;
        _invalid_header = other._invalid_header;
        _the_rest = other._the_rest;
    }
    return (*this);
}

HeaderAnalyzer::~HeaderAnalyzer()
{
}

void HeaderAnalyzer::parse_headers(const std::string& line,
                                   RequestHeaderState current_state)
{
    (void)line;
    (void)current_state;
}

std::string HeaderAnalyzer::host() const
{
    return (_host);
}

std::string HeaderAnalyzer::accept() const
{
    return (_accept);
}

std::string HeaderAnalyzer::accept_encoding() const
{
    return (_accept_encoding);
}

std::string HeaderAnalyzer::connection() const
{
    return (_connection);
}

ConnectionType HeaderAnalyzer::connection_type() const
{
    return (_connection_type);
}

std::string HeaderAnalyzer::content_length() const
{
    return (_content_length);
}

} // namespace webshell
