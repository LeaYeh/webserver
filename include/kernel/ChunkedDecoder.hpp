#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include "IDecoder.hpp"

class ChunkedDecoder : public IDecoder
{
  public:
    ChunkedDecoder();
    ChunkedDecoder(const ChunkedDecoder& other);
    ChunkedDecoder& operator=(const ChunkedDecoder& other);
    ~ChunkedDecoder();

    std::string decode(std::istream& is);

  private:
    int _hex_to_int(char c);
    std::string _decode_chunk(std::istream& is);
};