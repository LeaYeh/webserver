#include "Reactor.hpp"
#include "Acceptor.hpp"

namespace webkernel
{

class MainReactor : public Reactor
{
  public:
    MainReactor();
    MainReactor(const MainReactor&);
    MainReactor& operator=(const MainReactor&);
    ~MainReactor();

  private:
    Acceptor _acceptor;
};

} // namespace webkernel
