#include "IDecoder.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class ChunkedDecoder : public IDecoder
{
  public:
    ChunkedDecoder();
    ChunkedDecoder(const ChunkedDecoder& other);
    ChunkedDecoder& operator=(const ChunkedDecoder& other);
    ~ChunkedDecoder();

    std::string decode(std::istream& is)
    {
        std::ostringstream decoded_data;
        std::string chunk;

        while (!(chunk = _decode_chunk(is)).empty())
            decoded_data << chunk;
        return (decoded_data.str());
    }

  private:
    std::string _decode_chunk(std::istream& is)
    {
        std::string hex_length;
        std::getline(is, hex_length, '\r');
        is.ignore(1);

        std::stringstream ss(hex_length);
        int chunk_size;
        ss >> std::hex >> chunk_size;

        if (chunk_size == 0)
        {
            is.ignore(1);
            return "";
        }

        std::string chunk_data(chunk_size, 0);
        is.read(&chunk_data[0], chunk_size);
        is.ignore(2);

        return (chunk_data);
    }
};

/*
// usage example

ssize_t bytes_read = read(fd, buffer, CHUNKED_SIZE);
if (bytes_read > 0)
{
  if (_request_parser.header_analyzer().transfer_encoding() == CHUNKED &&
_request_parser.is_complete())
  {
    try
    {
      std::string decoded_body;

      if (_request_parser.partial_body().empty())
        decoded_body = decoder.decode(std::istringstream(std::string(buffer,
bytes_read))); else
      {
        decoded_body =
decoder.decode(std::istringstream(_request_parser.partial_body() +
std::string(buffer, bytes_read))); _request_parser.clean_partial_body();
      }
      if (!decoded_body.empty())
      {
        _processRequest(decoded_body);
        ...
      }
    }
    catch (const std::exception& e)
    {
      _handleClose(fd, weblog::ERROR, "Error while decoding chunked data: " +
std::string(e.what())); return;
    }
  }
  else
      ...
}
*/
