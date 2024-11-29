#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include <map>
#include <string>

namespace webkernel
{

class PostHandler : public ARequestHandler
{
  public:
    webshell::Response handle(int fd, EventProcessingState& state,
                              const webconfig::RequestConfig& config,
                              const webshell::Request& request);

  private:
    std::string _process(int fd, EventProcessingState& state,
                         const webconfig::RequestConfig& config,
                         const webshell::Request& request);
};

} // namespace webkernel
