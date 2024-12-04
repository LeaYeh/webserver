#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "UploadRecord.hpp"
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
    std::map<int, UploadRecord> _upload_record_pool;

  private:
    void _preProcess(const webconfig::RequestConfig& config,
                     const webshell::Request& request);
    std::string _process(int fd, EventProcessingState& state,
                         const webconfig::RequestConfig& config,
                         webshell::Request& request);
    void _postProcess(const webconfig::RequestConfig& config,
                      const webshell::Request& request,
                      const std::string& target_path,
                      const std::string& content);

    std::string _determine_file_name(const webshell::Request& request);
    std::string _determine_content_type(const webshell::Request& request);
    std::string _generate_safe_file_path(const webshell::Request& request);

    void _write_chunked_file(int fd, const std::string& content);
};

} // namespace webkernel
