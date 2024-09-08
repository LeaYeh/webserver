#include <zlib.h>
#include "IDecoder.hpp"

class GzipDecoder : public IDecoder
{
  public:
    GzipDecoder();
    GzipDecoder(const GzipDecoder& other);
    GzipDecoder& operator=(const GzipDecoder& other);
    ~GzipDecoder();

    std::string decode(std::istream& is);

  private:
    z_stream _zstream;
};
