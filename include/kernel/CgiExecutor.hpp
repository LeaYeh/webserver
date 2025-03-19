#pragma once
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "Singleton.hpp"
#include <cstddef>
#include <cstring>
#include <map>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

namespace webkernel
{
class CgiExecutor : public templates::Singleton<CgiExecutor>
{
public:
    static CgiExecutor* create_instance();
    void cgi_exec(webshell::Request& request, int client_fd);
    void remove_handler(int fd);
    bool handler_exists(int fd) const;

public:
    ~CgiExecutor();

private:
    std::string _script_path;
    std::string _script_name;
    std::string _path_info;
    std::map<int /* client_fd */, CgiHandler*> _handler_map;
    std::map<int /* client_fd */, int /* pipe_fd */> _pipe_map;

private:
    void _reset_path_meta(void);
    void _setup_path_meta(const std::string& route,
                          const std::string& path,
                          const std::string& cgi_path,
                          const std::string& cgi_extension);
    std::vector<std::string> _get_env(webshell::Request& request);
    char** _convert_to_str_array(std::vector<std::string> vec);
    void _free_array(char** arr, size_t size);
    void _close_all_fds(std::vector<int> vec);
    void _clean_handlers();
    const char** _construct_argv(const std::string& script_path, const std::string& ext);
    std::string _get_interpreter(const std::string& ext, char** env);
    void _debug_execve(const std::string& exec_path, char* const argv[], char* const env[]);
    void _busy_wait();

private:
    CgiExecutor();
    CgiExecutor(const CgiExecutor& other);
    CgiExecutor& operator=(const CgiExecutor& other);
};

} // namespace webkernel
