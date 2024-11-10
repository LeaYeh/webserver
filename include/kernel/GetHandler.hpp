#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"

namespace webkernel
{

class GetHandler : public ARequestHandler
{
  public:
    webshell::Response handle(const webconfig::RequestConfig& config,
                              const webshell::Request& request) const;
    std::map<std::string, std::string> responseHeaders(void) const;

  private:
    void _postProcess(const webconfig::RequestConfig& config,
                      const webshell::Request& request);
};

} // namespace webkernel
