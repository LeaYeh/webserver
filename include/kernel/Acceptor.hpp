#pragma once
#include "IHandler.hpp"
#include "Reactor.hpp"
#include <netdb.h>
#include <sys/socket.h>

namespace webkernel
{

class Acceptor : public IHandler
{
public:
    void handle_event(int fd, uint32_t events);

public:
    Acceptor(Reactor* reactor);
    Acceptor(const Acceptor& other);
    Acceptor& operator=(const Acceptor& other);
    ~Acceptor();

private:
    Reactor* _reactor;

private:
    Acceptor();
};

} // namespace webkernel
