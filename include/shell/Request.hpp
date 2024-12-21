#pragma once
#include "ChunkedCodec.hpp"
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
    Request();
    Request(std::string* buffer);
    Request(const Request&);
    Request& operator=(const Request&);
    ~Request();

    const RequestMethod& method() const;
    Uri uri() const;
    float version() const;
    const webconfig::RequestConfig& config() const;
    const std::map<std::string, std::string>& headers() const;
    const std::string& get_header(const std::string& name) const;
    const std::string serialize() const;
    bool empty_buffer() const;
    bool has_header(const std::string& name) const;

    bool read_chunked_body(std::vector<char>& chunked_body);

    void setMethod(RequestMethod method);
    void setUri(Uri uri);
    void setVersion(float version);
    void setHeaders(std::map<std::string, std::string> headers);
    // void setBody(std::string& body);
    void addHeader(std::string& name, std::string& value);
    void setReference(std::string* read_buffer);
    bool setupRequestConfig(int server_id);

  private:
    bool _proceed_content_len(std::vector<char>& chunked_body);
    bool _proceed_chunked(std::vector<char>& chunked_body);

    void _check_hexdigit(unsigned char c);
    void _check_size_crlf(unsigned char c);
    void _check_body(unsigned char c);
    void _check_body_crlf(unsigned char c);

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
    std::string* _read_buffer;
    webconfig::RequestConfig _config;
};

} // namespace webshell
