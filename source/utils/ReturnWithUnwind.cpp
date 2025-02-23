/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReturnWithUnwind.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 14:42:29 by mhuszar           #+#    #+#             */
/*   Updated: 2025/02/23 14:46:37 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ReturnWithUnwind.hpp"

ReturnWithUnwind::ReturnWithUnwind(int status)
{
    _status = status;
}

ReturnWithUnwind::~ReturnWithUnwind() throw()
{

}

ReturnWithUnwind::ReturnWithUnwind(const ReturnWithUnwind& other)
{
    this->_status = other._status;
}

int ReturnWithUnwind::status()
{
    return (_status);
}
