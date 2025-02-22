#include "CgiExecutor.hpp"
#include "CgiHandler.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "defines.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <sys/epoll.h>
#include <unistd.h>

extern char** environ;

namespace webkernel
{

CgiExecutor::CgiExecutor() {}

CgiExecutor::~CgiExecutor()
{
    LOG(weblog::DEBUG, "CgiExecutor destroyed");
}

std::string CgiExecutor::_replace_route(std::string route_path,
                                        const std::string& s1,
                                        const std::string& s2)
{
    if (s1.empty()) {
        return route_path;
    }

    size_t pos = 0;
    while ((pos = route_path.find(s1, pos)) != std::string::npos) {
        route_path.replace(pos, s1.length(), s2);
        pos += s2.length(); // Move past the replaced part
    }
    return route_path;
}

std::string CgiExecutor::_extract_path_info(const std::string& path,
                                            const std::string& cgi_path,
                                            const std::string& cgi_extension)
{
    size_t pos = path.find(cgi_path);
    if (pos == std::string::npos) {
        throw std::runtime_error("Base path not found in input path");
    }

    pos += cgi_path.length();
    if (pos >= path.length() || path[pos] != '/') {
        throw utils::HttpException(webshell::NOT_FOUND,
                                   "No Script-Name found in PATH.");
    }

    size_t next_slash = path.find('/', pos + 1);
    if (next_slash == std::string::npos) {
        return ""; // no PATH_INFO found
    }

    std::string _script_path = path.substr(0, next_slash);

    std::string script_name = path.substr(pos + 1, next_slash - pos - 1);

    if (script_name.length() < cgi_extension.size()
        || script_name.substr(script_name.length() - cgi_extension.size())
               != cgi_extension) {
        throw utils::HttpException(
            webshell::FORBIDDEN,
            "Script-Name does not have the correct extension.");
    }

    return path.substr(next_slash);
}

void CgiExecutor::_free_env(char** env, size_t size)
{
    if (env) {
        for (size_t i = 0; i < size; i++) {
            delete[] env[i];
        }
        delete[] env;
    }
}

char** CgiExecutor::_get_env(webshell::Request& request)
{
    std::vector<std::string> envp;
    for (int i = 0; environ[i] != NULL; i++) {
        envp.push_back(environ[i]);
    }

    if (request.method() == webshell::GET) {
        envp.push_back("REQUEST_METHOD = GET");
    }
    else if (request.method() == webshell::POST) {
        envp.push_back("REQUEST_METHOD = POST");
    }
    else {
        envp.push_back("REQUEST_METHOD = UNKNOWN");
    }

    envp.push_back("GATEWAY_INTERFACE = CGI/1.1");
    envp.push_back("SERVER_PROTOCOL = HTTP/1.1");
    envp.push_back("SERVER_SOFTWARE = webkernel/1.0");
    envp.push_back("SERVER_NAME" + request.config().server_name);
    envp.push_back("SERVER_PORT = " + request.uri().port);
    envp.push_back("REQUEST_URI = " + request.uri().path + "?"
                   + request.uri().query);
    envp.push_back("QUERY_STRING = " + request.uri().query);
    envp.push_back("SCRIPT_NAME = " + request.uri().path);
    envp.push_back("PATH_TRANSLATED = " + request.uri().path);
    envp.push_back("REMOTE_HOST = " + request.uri().host);

    std::string path_info = _extract_path_info(request.uri().path,
                                               request.config().cgi_path,
                                               request.config().cgi_extension);
    std::string path_translated = _replace_route(
        request.uri().path, request.config().route, request.config().cgi_path);
    if (path_info != "") {
        envp.push_back("PATH_INFO = " + path_info);
        envp.push_back("PATH_TRANSLATED = " + path_translated);
    }

    envp.push_back("SCRIPT_NAME = " + _script_path);

    char** env = NULL;

    try {
        env = new char*[envp.size() + 1];
        for (size_t i = 0; i < envp.size(); i++) {
            env[i] = new char[envp[i].size() + 1];
            if (env[i] == NULL) {
                throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                           "Failed to create new env 2");
            }
            strcpy(env[i], envp[i].c_str());
        }
        env[envp.size()] = NULL;
        return env;
    }
    catch (const std::bad_alloc& e) {
        _free_env(env, envp.size());
        return NULL;
    }
}

void CgiExecutor::cgi_exec(webshell::Request& request, int client_fd)
{
    (void)request;
    LOG(weblog::CRITICAL, "Handling CGI request...");
    int pipefd[2];
    IHandler* handler = NULL;

    if (pipe(pipefd) == -1) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Failed to create pipe");
    }
    try {
        pid_t pid = fork();

        // parent
        if (pid > 0) {
            handler = new CgiHandler(client_fd, pid);
            Reactor::instance()->register_handler(pipefd[0], handler, EPOLLIN | EPOLLOUT);
            close(pipefd[1]);

            // the monitor process
            pid_t pid_unwanted_child = fork();

            if (pid_unwanted_child == -1) {
                close(pipefd[0]);
                throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                           "Failed to fork");
            }
            if (pid_unwanted_child == 0) {
                close(pipefd[0]);
                sleep(3);
                kill(pid, SIGKILL);
                exit(SUCCESS);
            }
        }
        // child of the exec process
        else if (pid == 0) {
            // TODO: close all the file descriptors maybe by loop????
            close(pipefd[0]);
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                close(pipefd[1]);
                exit(FAILURE);
            }
            close(pipefd[1]);

            char* argv[2];
            argv[0] = strdup("hello_world.sh");
            argv[1] = NULL;
            // if (!execve(_script_path.c_str(), argv_null, environ)) {
            execve("/workspace/cgi-bin/hello_world.sh", argv, environ);
            perror(strerror(errno));
            sleep(100);
            exit(FAILURE);
        }
        else {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "Failed to fork");
        }
    }
    catch (std::exception& e) {
        Reactor::instance()->remove_handler(pipefd[0]);
        close(pipefd[0]);
        close(pipefd[1]);
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR, e.what());
    }
}

} // namespace webkernel
