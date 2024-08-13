#pragma once

#include <sys/socket.h>
#include <unistd.h>

namespace webkernel
{

class Acceptor
{
  public:
    Acceptor();
    Acceptor(const Acceptor&);
    Acceptor& operator=(const Acceptor&);
    ~Acceptor();

    int accept_connection();
    int listen_fd(void) const;

  private:
    int _listen_fd;
};

} // namespace webkernel
