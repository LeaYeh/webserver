/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 18:18:11 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/06 01:15:37 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "defines.hpp"
#include <string>

namespace webshell
{

struct Uri
{
    std::string raw;
    // std::string scheme;
    std::string authority;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    std::string fragment;
    URIType type;
    // std::string userinfo; //TODO: put test case
};

} // namespace webshell
