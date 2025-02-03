#include "IHandler.hpp"
#include "ConnectionHandler.hpp"
#include "Request.hpp"
#include "RequestProcessor.hpp"
#include "CgiHandler.hpp"
#include <cerrno>
#include <cstddef>
#include <unistd.h>
#include <cstring>

namespace webkernel
{
    class CgiExecution
    //public templates::Singleton<ConnectionHandler>
{
public:

    char** get_env(webshell::Request& request);
    void cgi_exec(webshell::Request &request, int client_fd);
    /*void handle_event(int fd, uint32_t events);
    void close_connection(int fd, weblog::LogLevel level, std::string message);
    void prepare_write(int fd, const std::string& buffer);
    void prepare_error(int fd, const utils::HttpException& e);
    bool get_session_data(const std::string& sid, std::string& out_data);
    bool set_session_data(const std::string& sid, const std::string& data);*/

public:
    ~CgiExecution();

private:
    std::map<int /*pid*/, CgiHandler*> _cgi_handlers;

private:
    CgiExecution();
    CgiExecution(const CgiExecution& other);
    CgiExecution& operator=(const CgiExecution& other);
};

} // namespace webkernel
