#include "CgiExecutor.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "ReturnWithUnwind.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <new>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

extern char** environ;

namespace webkernel
{

CgiExecutor::CgiExecutor() {}

CgiExecutor* CgiExecutor::create_instance()
{
    return (new CgiExecutor());
}

CgiExecutor::~CgiExecutor()
{
    std::map<int, CgiHandler*>::iterator iter = _handler_map.begin();
    while (iter != _handler_map.end()) {
        // Reactor::instance()->remove_handler(iter->first);
        delete iter->second;
        iter++;
    }
}

void CgiExecutor::cgi_exec(webshell::Request& request, int client_fd)
{
    (void)request;
    LOG(weblog::DEBUG, "Handling CGI request...");
    int pipefd[2];

    _setup_path_meta(request.config().route,
                     request.uri().path,
                     request.config().cgi_path,
                     request.config().cgi_extension);
    if (pipe(pipefd) == -1) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "Failed to create pipe");
    }

    try {
        pid_t pid = fork();

        // parent
        if (pid > 0) {
            _reset_path_meta();
            if (_handler_map.find(client_fd) != _handler_map.end())
                delete _handler_map[client_fd];
            _handler_map[client_fd] = new CgiHandler(client_fd, pid);
            _pipe_map[client_fd] = pipefd[0];
            Reactor::instance()->register_handler(
                pipefd[0], _handler_map[client_fd], EPOLLIN);
            close(pipefd[1]);

            // the monitor process
            pid_t pid_unwanted_child = fork();

            if (pid_unwanted_child == -1) {
                close(pipefd[0]);
                kill(pid, SIGKILL);
                throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                           "Failed to fork monitor process");
            }
            if (pid_unwanted_child == 0) {
                close(pipefd[0]);
                // sleep(3);
                _busy_wait();
                kill(pid, SIGKILL);
                throw ReturnWithUnwind(SUCCESS);
            }
        }
        // child of the exec process
        else if (pid == 0) {
            close(pipefd[0]);
            if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                close(pipefd[1]);
                throw ReturnWithUnwind(FAILURE);
            }
            close(pipefd[1]);

            if (request.method() == webshell::POST) {
                int fd = open(request.temp_file_path().c_str(), O_RDONLY);
                if (fd == -1) {
                    throw ReturnWithUnwind(FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    close(fd);
                    throw ReturnWithUnwind(FAILURE);
                }
                close(fd);
            }

            const char** argv =
                _construct_argv(_script_path, request.config().cgi_extension);
            char** env = _convert_to_str_array(_get_env(request));
            if (env == NULL || argv == NULL) {
                throw ReturnWithUnwind(FAILURE);
            }
            std::string interpreter =
                _get_interpreter(request.config().cgi_extension, env);
            std::vector<int> fd_vec = Reactor::instance()->get_active_fds();
            _close_all_fds(fd_vec);
            Reactor::instance()->destroy_tree();
            execve(interpreter.c_str(), (char* const*)argv, env);
            delete[] argv;
            delete[] env;
            throw ReturnWithUnwind(FAILURE);
        }
        else {
            throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                       "Failed to fork");
        }
    }
    catch (utils::HttpException& e) {
        Reactor::instance()->remove_handler(pipefd[0]);
        close(pipefd[0]);
        close(pipefd[1]);
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR, e.what());
    }
}

void CgiExecutor::_busy_wait()
{
    volatile size_t sleeper = 0;
    // while (sleeper < 10737418235)
    while (sleeper < 514748364)
        sleeper++;
}

void CgiExecutor::remove_handler(int fd)
{
    if (handler_exists(fd)) {
        // TODO: check if the handler need to be removed from the reactor, the
        // fd is the pipefd[0]
        LOG(weblog::DEBUG,
            "hehe remove here on fd: " + utils::to_string(_pipe_map[fd]));
        delete _handler_map[fd];
        _handler_map.erase(fd);
        Reactor::instance()->remove_handler(_pipe_map[fd]);
        _pipe_map.erase(fd);
    }
}

bool CgiExecutor::handler_exists(int fd) const
{
    return (_handler_map.find(fd) != _handler_map.end());
}

void CgiExecutor::_clean_handlers()
{
    std::map<int, CgiHandler*>::iterator iter = _handler_map.begin();
    while (iter != _handler_map.end()) {
        remove_handler(iter->first);
        iter++;
    }
}

void CgiExecutor::_close_all_fds(std::vector<int> vec)
{
    std::vector<int>::iterator iter = vec.begin();
    while (iter != vec.end()) {
        close(*iter);
        iter++;
    }
}

// std::string CgiExecutor::_replace_route(std::string route_path,
//                                         const std::string& s1,
//                                         const std::string& s2)
// {
//     if (s1.empty()) {
//         return route_path;
//     }

//     size_t pos = 0;
//     while ((pos = route_path.find(s1, pos)) != std::string::npos) {
//         route_path.replace(pos, s1.length(), s2);
//         pos += s2.length(); // Move past the replaced part
//     }
//     return route_path;
// }

void CgiExecutor::_reset_path_meta()
{
    _path_info.clear();
    _script_path.clear();
    _script_name.clear();
}

// setup will be setup in main process
void CgiExecutor::_setup_path_meta(const std::string& route,
                                   const std::string& path,
                                   const std::string& cgi_path,
                                   const std::string& cgi_extension)
{
    size_t pos = path.find(route);
    if (pos == std::string::npos) {
        throw utils::HttpException(
            webshell::INTERNAL_SERVER_ERROR,
            "A cgi request[" + path + "] cannot match cgi path: " + cgi_path);
    }
    pos += route.length();
    // TODO: If the route end with '/' in config file, the config parser need to
    // cry
    if (pos >= path.length() || path[pos] != '/') {
        throw utils::HttpException(
            webshell::INTERNAL_SERVER_ERROR,
            "A cgi request[" + path + "] cannot match cgi path: " + cgi_path);
    }
    size_t pos_end_of_script_name = path.find('/', pos + 1);
    if (pos_end_of_script_name != std::string::npos) {
        _path_info = path.substr(pos_end_of_script_name + 1, std::string::npos);
    }
    _script_name = path.substr(pos + 1, pos_end_of_script_name - (pos + 1));
    // _script_path = path.substr(0, pos_end_of_script_name);
    _script_path = utils::join(cgi_path, _script_name);

    if (_script_name.length() < cgi_extension.size()
        || _script_name.substr(_script_name.length() - cgi_extension.size())
               != cgi_extension) {
        throw utils::HttpException(
            webshell::FORBIDDEN,
            "Script-Name does not have the correct extension: " + _script_name
                + " " + cgi_extension);
    }

    if (access(_script_path.c_str(), F_OK) == -1) {
        throw utils::HttpException(webshell::NOT_FOUND,
                                   "Requested CGI script not found");
    }
    if (access(_script_path.c_str(), X_OK) == -1) {
        throw utils::HttpException(webshell::FORBIDDEN,
                                   "CGI script can not be executed");
    }

    LOG(weblog::DEBUG, "_script_path: " + _script_path);
    LOG(weblog::DEBUG, "_script_name: " + _script_name);
    LOG(weblog::DEBUG, "_path_info: " + _path_info);
}

void CgiExecutor::_free_array(char** arr, size_t size)
{
    if (arr) {
        for (size_t i = 0; i < size; i++) {
            delete[] arr[i];
        }
        delete[] arr;
    }
}

char** CgiExecutor::_convert_to_str_array(std::vector<std::string> vec)
{
    char** arr;

    try {
        arr = new char*[vec.size() + 1];
    }
    catch (std::bad_alloc& e) {
        return NULL;
    }
    for (size_t i = 0; i < vec.size(); i++) {
        int len = vec[i].size();
        try {
            arr[i] = new char[len + 1];
        }
        catch (std::bad_alloc& e) {
            _free_array(arr, i);
            return NULL;
        }
        std::strcpy(arr[i], vec[i].c_str());
    }
    arr[vec.size()] = NULL;
    return arr;
}

std::vector<std::string> CgiExecutor::_get_env(webshell::Request& request)
{
    std::vector<std::string> envp;

    for (int i = 0; environ[i] != NULL; i++) {
        envp.push_back(environ[i]);
    }
    if (request.method() == webshell::GET) {
        envp.push_back("REQUEST_METHOD=GET");
    }
    else if (request.method() == webshell::POST) {
        envp.push_back("REQUEST_METHOD=POST");
    }
    else {
        envp.push_back("REQUEST_METHOD=UNKNOWN");
    }

    // TODO: Check the define of each variable
    envp.push_back("PYTHONUNBUFFERED=1");
    envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envp.push_back("SERVER_SOFTWARE=webserv/1.0");
    envp.push_back("SERVER_NAME=" + request.config().server_name);
    envp.push_back("SERVER_PORT=" + request.uri().port);
    envp.push_back("REQUEST_URI=" + request.uri().path + "?"
                   + request.uri().query);
    envp.push_back("QUERY_STRING=" + request.uri().query);
    envp.push_back("SCRIPT_NAME=" + request.uri().path);
    envp.push_back("PATH_TRANSLATED=" + request.uri().path);
    envp.push_back("REMOTE_HOST=" + request.uri().host);
    envp.push_back("PATH_INFO=" + _path_info);

    if (request.method() == webshell::POST) {
        size_t file_size = get_file_size(request.temp_file_path());
        envp.push_back("CONTENT_LENGTH=" + utils::to_string(file_size));
    }
    else {
        envp.push_back("CONTENT_LENGTH=0");
    }

    return envp;
}

std::string CgiExecutor::_get_interpreter(const std::string& ext, char** env)
{
    std::string interpreter;
    std::vector<std::string> paths;
    const std::string name = ext == ".py" ? "python3" : "sh";

    for (int i = 0; env[i] != NULL; i++) {
        if (std::strncmp(env[i], "PATH=", 5) == 0) {
            std::string path = std::string(env[i] + 5);
            paths = utils::split(path, ':');
        }
        if (ext == ".py" && std::strncmp(env[i], "PYTHONPATH=", 11) == 0) {
            interpreter = std::string(env[i] + 11);
        }
        else if (ext == ".sh" && std::strncmp(env[i], "SHELL=", 6) == 0) {
            interpreter = std::string(env[i] + 6);
        }
    }
    if (interpreter.empty()) {
        for (size_t i = 0; i < paths.size(); i++) {
            std::string path = utils::join(paths[i], name);
            if (access(path.c_str(), X_OK) == 0) {
                interpreter = path;
                break;
            }
        }
    }
    return interpreter;
}

const char** CgiExecutor::_construct_argv(const std::string& script_path,
                                          const std::string& ext)
{
    const char** argv = new const char*[3];

    if (ext == ".py") {
        argv[0] = "python3";
    }
    else if (ext == ".sh") {
        argv[0] = "sh";
    }
    else {
        delete[] argv;
        return (NULL);
    }
    argv[1] = script_path.c_str();
    argv[2] = NULL;
    return (argv);
}

void CgiExecutor::_debug_execve(const std::string& exec_path,
                                char* const argv[],
                                char* const env[])
{
    LOG(weblog::WARNING, "==== Debug Execve Parameters ====");
    LOG(weblog::WARNING, "Executable Path: " + exec_path);
    LOG(weblog::WARNING, "Arguments:");
    for (int i = 0; argv[i] != NULL; i++) {
        LOG(weblog::WARNING, "  argv[" + utils::to_string(i) + "]: " + argv[i]);
    }

    LOG(weblog::WARNING, "Environment Variables:");
    for (int i = 0; env[i] != NULL; ++i) {
        LOG(weblog::WARNING, "  env[" + utils::to_string(i) + "]: " + env[i]);
    }
}

} // namespace webkernel
