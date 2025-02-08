#include "CgiExecution.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "defines.hpp"

extern char** environ;

namespace webkernel
{
    CgiExecution::CgiExecution() {}
    CgiExecution::~CgiExecution()
    {
        //clear_map(_cgi_handlers);
    }

std::string CgiExecution::_replace_route(std::string route_path, const std::string& s1, const std::string& s2) {
    if (s1.empty()) return route_path;

    size_t pos = 0;
    while ((pos = route_path.find(s1, pos)) != std::string::npos)
    {
        route_path.replace(pos, s1.length(), s2);
        pos += s2.length(); // Move past the replaced part
    }
    return route_path;
}

std::string CgiExecution::_extract_path_info(const std::string& path, const std::string& cgi_path, const std::string& cgi_extension)
{
    size_t pos = path.find(cgi_path);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Base path not found in input path"); //this should not happen
    }

    pos += cgi_path.length();
    if (pos >= path.length() || path[pos] != '/')
    {
        throw utils::HttpException(
            webshell::NOT_FOUND,
            "No Script-Name found in PATH.");
    }

    size_t next_slash = path.find('/', pos + 1);
    if (next_slash == std::string::npos)
    {
        return ""; //no PATH_INFO found
    }

    std::string _script_path = path.substr(0, next_slash);

    std::string script_name = path.substr(pos + 1, next_slash - pos - 1);
    
    if (script_name.length() < cgi_extension.size() || script_name.substr(script_name.length() - cgi_extension.size()) != cgi_extension)
    {
        throw utils::HttpException(
            webshell::FORBIDDEN,
            "Script-Name does not have the correct extension.");
    }

    return path.substr(next_slash);
}

char** CgiExecution::_get_env(webshell::Request& request)
{
    
    std::vector <std::string> envp;
    for (int i = 0; environ[i] != NULL; i++)
    {
        envp.push_back(environ[i]);
    }

    if (request.method() == webshell::GET)
    {
        envp.push_back("REQUEST_METHOD = GET");
    }
    else if (request.method() == webshell::POST)
    {
        envp.push_back("REQUEST_METHOD = POST");
    }
    else
    {
        envp.push_back("REQUEST_METHOD = UNKNOWN");
    }

    envp.push_back("GATEWAY_INTERFACE = CGI/1.1");
    envp.push_back("SERVER_PROTOCOL = HTTP/1.1");
    envp.push_back("SERVER_SOFTWARE = webkernel/1.0");
    envp.push_back("SERVER_NAME" + request.config().server_name);
    envp.push_back("SERVER_PORT = " + request.uri().port);
    envp.push_back("REQUEST_URI = " + request.uri().path + "?" + request.uri().query);
    envp.push_back("QUERY_STRING = " + request.uri().query);
    envp.push_back("SCRIPT_NAME = " + request.uri().path);
    envp.push_back("PATH_TRANSLATED = " + request.uri().path);
    envp.push_back("REMOTE_HOST = " + request.uri().host);

    std::string path_info = _extract_path_info(request.uri().path, request.config().cgi_path, request.config().cgi_extension);
    std::string path_translated = _replace_route(request.uri().path, request.config().route, request.config().cgi_path);
    if (path_info != "")
    {
        envp.push_back("PATH_INFO = " + path_info);
        envp.push_back("PATH_TRANSLATED = " + path_translated);
    }

    envp.push_back("SCRIPT_NAME = " + _script_path);

    char** env = new char*[envp.size()];
    for (size_t i = 0; i < envp.size(); i++)
    {
        env[i] = new char[envp[i].size() + 1];
        strcpy(env[i], envp[i].c_str());
    }
    env[envp.size()] = NULL;

    return env;
}

void CgiExecution::cgi_exec(webshell::Request &request, int client_fd)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        throw std::runtime_error("pipe() failed: " + std::string(strerror(errno)));
    }
    IHandler* handler_event = new CgiHandler(request, client_fd, pipefd[0]);
    Reactor::instance()->register_handler(pipefd[0], handler_event, EPOLLIN);
    pid_t pid = fork();

    if (pid > 0)
    {
        close(pipefd[1]);
        pid_t pid_unwanted_child = fork();
        if (pid_unwanted_child == 0)
        {
            sleep(3);
            kill(pid, SIGKILL);
            exit(0);
        }
    }
    else if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char** argv_null = NULL;
        char** environment = _get_env(request);
        execve(_script_path.c_str(), argv_null, environment);
        for (size_t i = 0; environment[i] != NULL; i++)
        {
            delete[] environment[i];
        }
        delete[] environment;
        //TODO: check the exit point, and fd leaks
        exit(-1);
    }
    else
    {
        throw std::runtime_error("fork() failed: " + std::string(strerror(errno)));
    }
}

} // namespace webkernel