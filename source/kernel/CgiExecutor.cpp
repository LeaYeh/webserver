#include "CgiExecutor.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "Reactor.hpp"
#include "ReturnWithUnwind.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cerrno>
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
            _handler_map[client_fd] = new CgiHandler(client_fd, pid);
            Reactor::instance()->register_handler(
                pipefd[0], _handler_map[client_fd], EPOLLIN);
            // _handler.reset(new CgiHandler(client_fd, pid));
            // Reactor::instance()->register_handler(
            //     pipefd[0], _handler.get(), EPOLLIN);
            close(pipefd[1]);

            // the monitor process
            pid_t pid_unwanted_child = fork();

            if (pid_unwanted_child == -1) {
                close(pipefd[0]);
                kill(pid, SIGKILL);
                // return ; //TODO: why does this not work?
                throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                           "Failed to fork monitor process");
            }
            if (pid_unwanted_child == 0) {
                close(pipefd[0]);
                sleep(3);
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

            const char* argv[2];
            argv[0] = _script_name.c_str();
            argv[1] = NULL;

            char** env = _convert_to_str_array(_get_env(request));
            if (env == NULL) {
                throw ReturnWithUnwind(FAILURE);
            }
            std::vector<int> fd_vec = Reactor::instance()->get_active_fds();
            _close_all_fds(fd_vec);
            Reactor::instance()->destroy_tree();
            execve(_script_path.c_str(), (char * const *)argv, env);
            // we catch this in the main so all destructors are called before
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

void CgiExecutor::remove_handler(int fd)
{
    if (handler_exists(fd)) {
        // TODO: check if the handler need to be removed from the reactor, the
        // fd is the pipefd[0]
        // Reactor::instance()->remove_handler(fd);
        delete _handler_map[fd];
        _handler_map.erase(fd);
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
        throw std::runtime_error("A cgi request[" + path
                                 + "] cannot match cgi path: " + cgi_path);
    }
    pos += route.length();
    // TODO: If the route end with '/' in config file, the config parser need to
    // cry
    if (pos >= path.length() || path[pos] != '/') {
        throw std::runtime_error("A cgi request[" + path
                                 + "] cannot match cgi path: " + cgi_path);
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
            "Script-Name does not have the correct extension.");
    }

    if (access(_script_path.c_str(), F_OK) == -1)
        throw utils::HttpException(
            webshell::NOT_FOUND,
            "Requested CGI script not found");
    if (access(_script_path.c_str(), X_OK) == -1)
        throw utils::HttpException(
            webshell::FORBIDDEN,
            "CGI script can not be executed");

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

} // namespace webkernel
