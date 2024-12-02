/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedCodec.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/29 21:14:28 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/29 21:14:33 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ICodec.hpp"
#include <string>

namespace webkernel
{

class ChunkedCodec : public ICodec
{
  public:
    ChunkedCodec();
    ChunkedCodec(const ChunkedCodec& other);
    ChunkedCodec& operator=(const ChunkedCodec& other);
    ~ChunkedCodec();

    std::string decode(const std::string& content) const;
    std::string decode_single(std::string& content) const;
    std::string encode(const std::string& content) const;
    // bool is_valid(const std::string& content) const;
};

} // namespace webkernel

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
