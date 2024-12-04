#pragma once
#include "RequestConfig.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include "ChunkedCodec.hpp"
#include <map>
#include <string>


namespace webshell
{

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
    const std::map<std::string, std::string>& headers() const;
    const std::string& get_header(const std::string& name) const;
    const std::string serialize() const;
    bool has_header(const std::string& name) const;

    bool read_chunked_body(std::string& chunked_body);

    void setMethod(RequestMethod method);
    void setUri(Uri uri);
    void setVersion(float version);
    void setHeaders(std::map<std::string, std::string> headers);
    void setBody(std::string& body);
    void addHeader(std::string& name, std::string& value);
    void setReferences(webconfig::RequestConfig* config, std::string* read_buffer);

  private:

    bool _proceed_content_len(std::string& chunked_body);
    bool _proceed_chunked(std::string& chunked_body);
  
    webkernel::ChunkedCodec _codec;
    size_t _processed;
  
    RequestMethod _method;
    Uri _uri;
    float _version;
    std::map<std::string, std::string> _headers;
    std::string* _read_buffer;
    webconfig::RequestConfig* _config;
};

} // namespace webshell
