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
    std::string decode(const std::string& content) const;
    std::string decode_single(std::string& content) const;
    std::string encode(const std::string& content) const;

public:
    ChunkedCodec();
    ChunkedCodec(const ChunkedCodec& other);
    ChunkedCodec& operator=(const ChunkedCodec& other);
    ~ChunkedCodec();
};

} // namespace webkernel
