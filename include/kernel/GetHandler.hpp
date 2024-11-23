#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include <map>

namespace webkernel
{

class GetHandler : public ARequestHandler
{
  public:
    webshell::Response handle(int fd, EventProcessingState& state,
                              const webconfig::RequestConfig& config,
                              const webshell::Request& request);

  private:
    std::map</* conn fd */ int, /* file pos */ std::streampos>
        _chunked_file_records;
    std::string _target_path;

    void _handle_standard(EventProcessingState& state,
                          const std::string& target_path,
                          std::string& content) const;
    void _handle_chunked(int fd, EventProcessingState& state,
                         const std::string& target_path, std::string& content);
    void _handle_autoindex(EventProcessingState& state,
                           const std::string& request_path,
                           const webconfig::RequestConfig& config,
                           std::string& content);
};

} // namespace webkernel
