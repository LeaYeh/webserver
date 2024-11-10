#pragma once
#include "AConfigParser.hpp"
#include "ConfigLocationBlock.hpp"
#include "Request.hpp"
#include "Response.hpp"

namespace webkernel
{

class ARequestHandler
{
  public:
    virtual ~ARequestHandler();
    virtual webshell::Response
    handle(const webconfig::AConfigParser* config,
           const webshell::Request& request) const = 0;
    virtual std::map<std::string, std::string> responseHeaders(void) const = 0;

    bool checkPathFormat(const std::string& path);
    bool checkMethodLimit(webshell::RequestMethod method,
                          const std::vector<webshell::RequestMethod>& limit);
    webconfig::ConfigLocationBlock*
    matchRequestPath(const webshell::Request& request, int server_id);

  protected:
    std::map<std::string, std::string> _response_headers;
    virtual bool _preProcess(const webconfig::AConfigParser* config,
                             const webshell::Request& request) = 0;
    virtual bool _postProcess(webconfig::AConfigParser* config,
                              webshell::Request& request) = 0;
};

} // namespace webkernel
