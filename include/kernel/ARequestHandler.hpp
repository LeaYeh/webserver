#pragma once
#include "ChunkedCodec.hpp"
#include "Request.hpp"
#include "RequestConfig.hpp"
#include "Response.hpp"
#include "TemplateEngine.hpp"
#include "defines.hpp"
#include <map>
#include <string>

namespace webkernel
{

class ARequestHandler
{
  public:
    ARequestHandler();
    ARequestHandler(const ARequestHandler& other);
    ARequestHandler& operator=(const ARequestHandler& other);
    virtual ~ARequestHandler();
    virtual webshell::Response handle(int fd, EventProcessingState& state,
                                      webshell::Request& request) = 0;

  protected:
    std::map<std::string, std::string> _response_headers;
    ChunkedCodec _chunked_codec;
    TemplateEngine _template_engine;
    std::string _target_path;

    bool _checkPathFormat(const std::string& path) const;
    bool
    _checkMethodLimit(webshell::RequestMethod method,
                      const std::vector<webshell::RequestMethod>& limit) const;
    bool _is_out_of_max_file_size(const webconfig::RequestConfig& config,
                                  const std::string file_path) const;
    int _get_respones_encoding(const webshell::Request& request) const;
    std::string _get_encoding_string(int encoding) const;

    std::string _get_resource_path(const webconfig::RequestConfig& config,
                                   const std::string& path) const;

    const std::string _getMimeType(const std::string& file_path) const;

    virtual std::string _process(int fd, EventProcessingState& state,
                                 webshell::Request& request) = 0;
    virtual void _postProcess(const webshell::Request& request,
                              const std::string& target_path,
                              const std::string& content) = 0;
    virtual void _preProcess(const webshell::Request& request);
};

} // namespace webkernel
