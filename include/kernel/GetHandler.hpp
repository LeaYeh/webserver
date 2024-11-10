#pragma once
#include "ARequestHandler.hpp"

namespace webkernel
{

class GetHandler : public ARequestHandler
{
  public:
    webshell::Response handle(const webconfig::AConfigParser* config,
                              const webshell::Request& request) const;
    std::map<std::string, std::string> responseHeaders(void) const;

  private:
    bool _preProcess(const webconfig::AConfigParser* config,
                     const webshell::Request& request);
    bool _postProcess(webconfig::AConfigParser* config,
                      webshell::Request& request);
};

} // namespace webkernel
