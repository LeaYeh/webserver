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
                              webshell::Request& request);

  private:
    std::string _process(int fd, EventProcessingState& state,
                         const webconfig::RequestConfig& config,
                         webshell::Request& request);
    std::string _determine_file_name(const webshell::Request& request);
    std::string _determine_content_type(const webshell::Request& request);
    std::string _generate_safe_file_path(const std::string& file_name);
    void _write_chunked_file(const std::string& file_path, const std::string& content);
};

} // namespace webkernel
