#pragma once

#include "Reactor.hpp"
#include "Handler.hpp"

namespace webkernel
{

class SubReactor : public Reactor
{
  public:
    SubReactor();
    SubReactor(const SubReactor&);
    SubReactor& operator=(const SubReactor&);
    ~SubReactor();

  private:
    Handler _handler;

};

} // namespace webkernel

