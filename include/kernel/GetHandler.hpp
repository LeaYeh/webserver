#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"
#include <map>
#include <string>

namespace webkernel
{

class GetHandler : public ARequestHandler
{
public:
    webshell::Response
    handle(int fd, EventProcessingState& state, webshell::Request& request);

public:
    GetHandler();
    ~GetHandler();

private:
    std::map</* conn fd */ int, /* file pos */ std::streampos>
        _chunked_file_records;

private:
    std::string
    _process(int fd, EventProcessingState& state, webshell::Request& request);
    void _preProcess(const webshell::Request& request);
    void _postProcess(const webshell::Request& request,
                      const std::string& target_path,
                      const std::string& content);

    void _handle_standard(EventProcessingState& state,
                          const std::string& target_path,
                          std::string& content) const;
    void _handle_chunked(int fd,
                         EventProcessingState& state,
                         const std::string& target_path,
                         std::string& content);
    void _handle_autoindex(EventProcessingState& state,
                           const std::string& request_path,
                           const webconfig::RequestConfig& config,
                           std::string& content);

private:
    GetHandler(const GetHandler& other);
    GetHandler& operator=(const GetHandler& other);
};

} // namespace webkernel
