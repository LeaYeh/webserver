#include "ChunkedCodec.hpp"
#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include <sstream>

namespace webkernel
{

ChunkedCodec::ChunkedCodec()
{
}

ChunkedCodec::ChunkedCodec(const ChunkedCodec& other) : ICodec(other)
{
}

ChunkedCodec& ChunkedCodec::operator=(const ChunkedCodec& other)
{
    if (this != &other)
    {
        ICodec::operator=(other);
    }
    return *this;
}

ChunkedCodec::~ChunkedCodec()
{
}

std::string ChunkedCodec::encode(const std::string& content) const
{
    std::string encoded_content;
    std::ostringstream oss;

    oss << std::hex << content.size() << "\r\n" << content << "\r\n";
    encoded_content = oss.str();
    return (encoded_content);
}

/*
// usage example
7\n\r
Mozilla\n\r
9\n\r
Developer\n\r
7\n\r
Network\n\r
0\n\r
*/
std::string ChunkedCodec::decode(const std::string& content) const
{
    // TODO: Check the payload size outside of this function
    // webconfig::Config* config = webconfig::Config::instance();
    // if (content.size() > config->httpBlock().clientMaxBodySize())
    //     throw utils::HttpException(
    //         webshell::PAYLOAD_TOO_LARGE,
    //         "Chunked data size exceeds client_max_body_size");
    std::string decoded_content;
    size_t pos = 0;
    size_t newline_pos = 0;

    while (true)
    {
        newline_pos = content.find("\r\n", pos);
        if (newline_pos == std::string::npos)
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Invalid chunked data format");
        std::string chunk_size_str = content.substr(pos, newline_pos - pos);
        size_t chunk_size = 0;

        try
        {
            std::istringstream(chunk_size_str) >> std::hex >> chunk_size;
        }
        catch (std::exception& e)
        {
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Invalid chunked size format: " +
                                           chunk_size_str);
        }

        if (chunk_size == 0)
        {
            if (content.substr(newline_pos + 2, 2) != "\r\n")
                throw utils::HttpException(webshell::BAD_REQUEST,
                                           "Chunked data should end with CRLF");
            break;
        }
        pos = newline_pos + 2;
        if (pos + chunk_size > content.size())
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Chunked data size mismatch");
        decoded_content += content.substr(pos, chunk_size);
        pos += chunk_size;
        if (content.substr(pos, 2) != "\r\n")
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Each chunk should end with CRLF");
        pos += 2;
    }
    return (decoded_content);
}

} // namespace webkernel
