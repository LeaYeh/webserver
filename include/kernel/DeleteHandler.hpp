#pragma once
#include "ARequestHandler.hpp"
#include "defines.hpp"

namespace webkernel
{

class DeleteHandler : public ARequestHandler
{
public:
    webshell::Response
    handle(int fd, EventProcessingState& state, webshell::Request& request);
    DeleteHandler();
    ~DeleteHandler();

private:
    std::string
    _process(int fd, EventProcessingState& state, webshell::Request& request);
    void _pre_process(const webshell::Request& request);
    void _post_process(const webshell::Request& request,
                       const std::string& target_path,
                       const std::string& content);

private:
    DeleteHandler(const DeleteHandler& other);
    DeleteHandler& operator=(const DeleteHandler& other);
};
} // namespace webkernel
