#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include <map>

namespace webkernel
{

class PostHandler : public ARequestHandler
{
  public:
    webshell::Response handle(int fd, EventProcessingState& state,
                              const webconfig::RequestConfig& config,
                              const webshell::Request& request);

  private:

};

} // namespace webkernel

