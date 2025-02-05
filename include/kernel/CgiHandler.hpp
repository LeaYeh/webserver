#include "Request.hpp"
#include <cctype>
#include <sys/epoll.h>
#include "IHandler.hpp"

namespace webkernel
{
    class CgiHandler : public IHandler
    {
    public:
        CgiHandler(webshell::Request& request, int client_fd, int read_end);
        ~CgiHandler();
        void handle_event(int fd, uint32_t events);

    private:
        webshell::Request& _request;
        std::string _buffer;
        int _client_fd;
        int _read_end;
    };
} // namespace webkernel
