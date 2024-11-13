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
7
Mozilla
9
Developer
7
Network
0
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
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line, '\n'))
    {
        // should end with '\r\n'
        if (line[line.size() - 1] != '\r')
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Chunked data should end with \\r\\n");
        line.erase(line.size() - 1);

        size_t chunk_size = 0;
        std::istringstream(line) >> std::hex >> chunk_size;

        if (chunk_size == 0)
        {
            weblog::Logger::log(weblog::DEBUG,
                                "ChunkedCodec::decode: end of chunked data");
            if (std::getline(iss, line, '\n'))
            {
                if (line != "\r")
                    throw utils::HttpException(
                        webshell::BAD_REQUEST,
                        "Chunked data should end with \\r\\n");
            }
            break;
        }
        std::string chunk;
        chunk.resize(chunk_size);
        // check the read size need to be equal to chunk_size
        if (!iss.read(&chunk[0], chunk_size))
            throw utils::HttpException(webshell::BAD_REQUEST,
                                       "Chunked data size mismatch");

        decoded_content += chunk;
    }
    return (decoded_content);
}

} // namespace webkernel
