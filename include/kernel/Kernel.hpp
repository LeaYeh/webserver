#pragma once
#include "Acceptor.hpp"
#include "Reactor.hpp"
#include "SessionManager.hpp"
#include <netdb.h>
#include <sys/socket.h>

namespace webkernel
{

class Kernel
{
public:
    Kernel();
    Kernel(const Kernel& other);
    Kernel& operator=(const Kernel& other);
    ~Kernel();

    void run(void);

private:
    Acceptor* _acceptor;
    SessionManager* _session_manager;

    void _register_listener(void);
    int _create_listen_socket(const char* ip, const char* port);
};

} // namespace webkernel
