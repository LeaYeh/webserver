#include "IHandler.hpp"
#include "ConnectionHandler.hpp"
#include "Request.hpp"
#include "RequestProcessor.hpp"
#include "CgiHandler.hpp"
#include <cerrno>
#include <cstddef>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <iostream>

namespace webkernel
{
    class CgiExecution
{
public:

    void cgi_exec(webshell::Request &request, int client_fd);

public:
    ~CgiExecution();

private:
    std::map<int /*pid*/, CgiHandler*> _cgi_handlers;
    std::string _script_path;

private:
    CgiExecution();
    CgiExecution(const CgiExecution& other);
    CgiExecution& operator=(const CgiExecution& other);

    std::string _replace_route(std::string route_path, const std::string& s1, const std::string& s2);
    std::string _extract_path_info(const std::string& path, const std::string& cgi_path, const std::string& cgi_extension);
    char** _get_env(webshell::Request& request);
    void _free_env(char** env, size_t size);
};

} // namespace webkernel
