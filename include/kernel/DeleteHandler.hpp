#pragma once
#include "ARequestHandler.hpp"
#include "RequestConfig.hpp"
#include "defines.hpp"

namespace webkernel
{

class DeleteHandler : public ARequestHandler
{
public:
    webshell::Response
    handle(int fd, EventProcessingState& state, webshell::Request& request);

private:
    std::string
    _process(int fd, EventProcessingState& state, webshell::Request& request);
    void _pre_process(const webshell::Request& request);
    void _post_process(const webshell::Request& request,
                       const std::string& target_path,
                       const std::string& content);
};
} // namespace webkernel
