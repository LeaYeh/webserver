#include "IHandler.hpp"
#include "Request.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

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
    std::string _script_name;
    std::string _path_info;

private:
    CgiExecutor(const CgiExecutor& other);
    CgiExecutor& operator=(const CgiExecutor& other);

    // std::string _replace_route(std::string route_path,
    //                            const std::string& s1,
    //                            const std::string& s2);
    void _reset_path_meta(void);
    void _setup_path_meta(const std::string& route,
                          const std::string& path,
                          const std::string& cgi_path,
                          const std::string& cgi_extension);
    std::vector<std::string> _get_env(webshell::Request& request);
    char** _convert_to_str_array(std::vector<std::string> vec);
    void _free_array(char** arr, size_t size);

    IHandler* _handler;
};

} // namespace webkernel
