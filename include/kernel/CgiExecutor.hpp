#include "IHandler.hpp"
#include "Request.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include "Response.hpp"

namespace webkernel
{
class CgiExecutor
{
public:
    void cgi_exec(webshell::Request& request, int client_fd);
    // webshell::Response cgi_exec(webshell::Request& request, int client_fd);

public:
    CgiExecutor();
    ~CgiExecutor();

private:
    std::string _script_path;

private:
    CgiExecutor(const CgiExecutor& other);
    CgiExecutor& operator=(const CgiExecutor& other);

    std::string _replace_route(std::string route_path,
                               const std::string& s1,
                               const std::string& s2);
    std::string _extract_path_info(const std::string& path,
                                   const std::string& cgi_path,
                                   const std::string& cgi_extension);
    char** _get_env(webshell::Request& request);
    void _free_env(char** env, size_t size);

    IHandler* _handler;
};

} // namespace webkernel
