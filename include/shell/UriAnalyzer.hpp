#pragma once

#include "AParser.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include <string>

namespace webshell
{

class UriAnalyzer : public AParser
{
public:
    void parse_uri(std::string& uri);
    Uri take_uri() const;
    void reset();

public:
    UriAnalyzer();
    UriAnalyzer(const UriAnalyzer& other);
    UriAnalyzer& operator=(const UriAnalyzer& other);
    ~UriAnalyzer();

private:
    std::string _uri;
    std::string _host;
    std::string _port;
    std::string _path;
    std::string _query;
    std::string _fragment;
    std::string _temp_buf;
    URIState _state;
    URIType _type;
    unsigned int _idx;
    int _sidx;
    bool _ipv_digit;
    int _ipv_dot;

private:
    void _feed(unsigned char c);

    void _uri_start(unsigned char c);
    void _uri_limbo(unsigned char c);
    void _uri_rel_start(unsigned char c);
    void _uri_scheme(unsigned char c);
    void _uri_host_trial(unsigned char c);
    void _uri_host_ipv6(unsigned char c);
    void _uri_after_ipv6(unsigned char c);
    void _uri_host_regname(unsigned char c);
    void _uri_port(unsigned char c);
    void _uri_path_trial(unsigned char c);
    void _uri_path(unsigned char c);
    void _uri_query(unsigned char c);
    void _uri_fragment(unsigned char c);

    unsigned char _decode_percent(std::string& str);
    unsigned char _decode_num_and_alpha();
    void _decode(std::string& str);
    void _percent_decode_all();
    unsigned char _hexval(unsigned char c);
    bool _valid_hexdigit(unsigned char c);

    std::string _remove_dot_segments() const;
    void _remove_last_segment(std::string& str) const;
    void _move_first_segment(std::string& from, std::string& to) const;
};
} // namespace webshell
