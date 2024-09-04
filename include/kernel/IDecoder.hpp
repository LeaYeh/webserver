#include <iostream>
#include <istream>
#include <string>

class IDecoder
{
  public:
    virtual ~IDecoder();

    virtual std::string decode(std::istream& is) = 0;
};
