/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URIMachine.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 21:10:19 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/05 20:59:45 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "defines.hpp"
#include "HttpException.hpp"
#include "URIMachine.hpp"
#include "ParseException.hpp"


namespace webshell
{

URIMachine::URIMachine()
{
    
}

URIMachine::~URIMachine() {}

void URIMachine::feed(unsigned char c)
{
    (void)c;
}

bool URIMachine::done()
{
    return (_machine.done());
}

} // namespace webshell
