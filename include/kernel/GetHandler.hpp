#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"

namespace webkernel
{

class GetHandler : public ARequestHandler
{
  public:
    webshell::Response handle(const webconfig::RequestConfig& config,
                              const webshell::Request& request);

  private:
    void _handle_standard(const std::string& target_path,
                          std::stringstream& content);
    void _handle_autoindex(const webconfig::RequestConfig& config,
                           const std::string& target_path);
    void _handle_chunked(const webconfig::RequestConfig& config,
                         const std::string& target_path);
};

} // namespace webkernel
