#pragma once
#include "ChunkedCodec.hpp"
#include "ConfigServerBlock.hpp"
#include "RequestConfig.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include <cstddef>
#include <map>
#include <string>

namespace webshell
{

#define CRLF "\r\n"

class Request
{
public:
    const RequestMethod& method() const;
    Uri uri() const;
    float version() const;
    const webconfig::RequestConfig& config() const;
    const std::map<std::string, std::string>& headers() const;
    const std::map<std::string, std::string>& cookies() const;
    const std::string& get_header(const std::string& name) const;
    const std::string& get_cookie(const std::string& name) const;
    const std::string serialize() const;
    bool empty_buffer() const;
    bool has_header(const std::string& name) const;
    bool has_cookie(const std::string& name) const;
    bool has_cookies() const;
    bool read_chunked_body(std::vector<char>& chunked_body);

    std::string read_chunked_body();

    void set_method(RequestMethod method);
    void set_uri(Uri uri);
    void set_version(float version);
    void set_headers(std::map<std::string, std::string> headers);
    void add_header(std::string& name, std::string& value);
    void set_reference(std::string* read_buffer);
    void setup_config(webconfig::ConfigServerBlock* server_config);
    void set_cookies(std::map<std::string, std::string> cookies);
    const std::string& temp_file_path() const;
    bool is_cgi() const;

public:
    Request();
    Request(std::string* buffer);
    Request(const Request&);
    Request& operator=(const Request&);
    ~Request();

private:
    webkernel::ChunkedCodec _codec;
    size_t _processed;
    ChunkState _state;
    std::string _chunksize;
    size_t _size_num;
    std::vector<char> _chunkbuf;

    RequestMethod _method;
    Uri _uri;
    float _version;
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _cookies;
    std::string* _read_buffer;
    webconfig::RequestConfig _config;
    std::string _temp_file_path;

private:
    bool _proceed_content_len(std::vector<char>& chunked_body);
    bool _proceed_chunked(std::vector<char>& chunked_body);
    void _write_chunked_file(const std::vector<char>& content);

    void _check_hexdigit(unsigned char c);
    void _check_size_crlf(unsigned char c);
    void _check_body(unsigned char c);
    void _check_body_crlf(unsigned char c);
    std::string _generate_temp_file_path();
};

} // namespace webshell
