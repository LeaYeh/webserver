#pragma once
#include "Request.hpp"
#include "RequestConfig.hpp"
#include "Response.hpp"
#include "defines.hpp"
#include <utility>

namespace webkernel
{

class ARequestHandler
{
  public:
    virtual ~ARequestHandler();
    virtual webshell::Response
    handle(const webconfig::RequestConfig& config,
           const webshell::Request& request) const = 0;
    virtual std::map<std::string, std::string> responseHeaders(void) const = 0;

  protected:
    std::map<std::string, std::string> _response_headers;

    bool _checkPathFormat(const std::string& path) const;
    bool
    _checkMethodLimit(webshell::RequestMethod method,
                      const std::vector<webshell::RequestMethod>& limit) const;

    std::pair<webshell::StatusCode, std::string>
    _preProcess(const webconfig::RequestConfig& config,
                const webshell::Request& request) const;
    void _postProcess(const webconfig::RequestConfig& config,
                      const webshell::Request& request);
};

} // namespace webkernel
