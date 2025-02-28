#pragma once
#include "ARequestHandler.hpp"
#include "UploadRecord.hpp"
#include "defines.hpp"
#include <map>
#include <string>

namespace webkernel
{

class PostHandler : public ARequestHandler
{
public:
    webshell::Response
    handle(int fd, EventProcessingState& state, webshell::Request& request);

public:
    PostHandler();
    ~PostHandler();

private:
    std::string _upload_file_path;
    std::string _temp_file_path;

private:
    void _pre_process(const webshell::Request& request);
    std::string
    _process(int fd, EventProcessingState& state, webshell::Request& request);
    void _post_process(const webshell::Request& request,
                       const std::string& target_path,
                       const std::string& content);

    std::string _determine_file_name(const webshell::Request& request);
    std::string _determine_content_type(const webshell::Request& request);
    std::string _generate_safe_file_path(const webshell::Request& request);

    webshell::Response _handle_completed(const webshell::Request& request);
    bool _already_exist(const std::string& file_path);
    void _check_upload_permission(const webshell::Request& request);

private:
    PostHandler(const PostHandler& other);
    PostHandler& operator=(const PostHandler& other);
};

} // namespace webkernel
