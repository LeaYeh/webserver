#pragma once
#include "Acceptor.hpp"
#include "Reactor.hpp"
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
    Reactor* _reactor;
    Acceptor* _acceptor;

    void _registerListener(void);
    int _createListenSocket(const char* ip, const char* port);
};

} // namespace webkernel
