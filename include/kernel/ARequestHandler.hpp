#pragma once
#include "CgiExecutor.hpp"
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
    virtual webshell::Response
    handle(int fd, EventProcessingState& state, webshell::Request& request) = 0;

public:
    ARequestHandler();
    ARequestHandler(const ARequestHandler& other);
    ARequestHandler& operator=(const ARequestHandler& other);
    virtual ~ARequestHandler();

protected:
    std::map<std::string, std::string> _response_headers;
    ChunkedCodec _chunked_codec;
    TemplateEngine _template_engine;
    std::string _target_path;
    EventProcessingState _state;

protected:
    bool _check_path_format(const std::string& path) const;
    bool _check_method_limit(
        webshell::RequestMethod method,
        const std::vector<webshell::RequestMethod>& limit) const;
    bool _is_out_of_max_file_size(const webconfig::RequestConfig& config,
                                  const std::string file_path) const;
    virtual bool _check_path_permission(const std::string& path,
                                        const int type) const;
    int _get_respones_encoding(const webshell::Request& request,
                               const std::string resource_path) const;
    std::string _get_encoding_string(int encoding) const;
    std::string _get_resource_path(const webconfig::RequestConfig& config,
                                   const std::string& path) const;
    const std::string _get_mime_type(const std::string& file_path) const;

    void _handle_exception(
        const std::exception& e,
        const webshell::StatusCode code = webshell::INTERNAL_SERVER_ERROR,
        const webshell::ContentType content_type = webshell::TEXT_HTML);
    void _update_status(EventProcessingState& state,
                        EventProcessingState flags,
                        bool overwrite = false) const;
    virtual std::string _process(int fd,
                                 EventProcessingState& state,
                                 webshell::Request& request) = 0;
    virtual void _pre_process(const webshell::Request& request);
    virtual void _post_process(const webshell::Request& request,
                               const std::string& target_path,
                               const std::string& content) = 0;
};

} // namespace webkernel
