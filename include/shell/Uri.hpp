/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 18:18:11 by mhuszar           #+#    #+#             */
/*   Updated: 2025/01/03 20:12:48 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
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
    bool _origin_form;
    // std::string userinfo; //TODO: put test case
};

} // namespace webshell
