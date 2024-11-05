/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URIMachine.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 21:10:19 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/05 21:00:01 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URIMACHINE_HPP
# define URIMACHINE_HPP

#include "defines.hpp"
#include "StateMachine.hpp"

#define MACHINE_DEBUG 1

namespace webshell
{

class URIMachine
{
    public:
        URIMachine();
        ~URIMachine();

        void feed(unsigned char c);
        bool done(void);

    private:
        URIMachine(const URIMachine& other);
        URIMachine& operator=(const URIMachine& other);

        StateMachine<URIState> _machine;
};

} // namespace webshell

#endif
