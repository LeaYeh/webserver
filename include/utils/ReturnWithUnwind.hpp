/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReturnWithUnwind.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 14:42:31 by mhuszar           #+#    #+#             */
/*   Updated: 2025/02/23 14:45:43 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNWIND_RETURN_HPP
#define UNWIND_RETURN_HPP

#include <exception>

class ReturnWithUnwind : public std::exception
{

public:
    ReturnWithUnwind(int status);
    ~ReturnWithUnwind() throw();
    ReturnWithUnwind(const ReturnWithUnwind& other);
    int status();

private:
    ReturnWithUnwind(void);
    ReturnWithUnwind& operator=(const ReturnWithUnwind& other);

    int _status;
};

#endif
