#pragma once

#include "defines.hpp"
#include <string>

namespace webshell
{

class UriAnalyzer
{
  public:
    UriAnalyzer();
    UriAnalyzer(const UriAnalyzer& other);
    UriAnalyzer& operator=(const UriAnalyzer& other);
    ~UriAnalyzer();

    void parse_uri(std::string& uri);
    void reset();

    // std::string scheme(void) const;
    // std::string directory(void) const;
    std::string query(void) const;

  private:

    void _feed(unsigned char c);

    void _uri_start(unsigned char c);
    void _uri_rel_start(unsigned char c);
    void _uri_scheme(unsigned char c);
    void _uri_port(unsigned char c);
    void _uri_path_trial(unsigned char c);
    void _uri_path(unsigned char c);
    void _uri_query(unsigned char c);
    void _uri_fragment(unsigned char c);
    void _uri_host_trial(unsigned char c);
    
    bool _is_gen_delim(unsigned char c);
    bool _is_sub_delim(unsigned char c);
    bool _is_unreserved(unsigned char c);
    bool _is_pchar(unsigned char c/*, bool userinfo*/);
    bool _is_query_or_fragment_part(unsigned char c);

    unsigned char _decode_percent();
    unsigned char _hexval(unsigned char c);
    bool _valid_hexdigit(unsigned char c);

    // std::string _scheme; //we dont really need this for anything
    std::string _uri;
    std::string _host;
    std::string _port;
    std::string _path;
    
    // std::string _directory;
    std::string _query;
    std::string _fragment;

    URIState _state;
    unsigned int _idx;
    // int _max;
    int _sidx;
    int _percent_idx;
    int _percent_val;
};
} // namespace webshell
