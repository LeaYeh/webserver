/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RLMachines.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 21:10:19 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/01 18:19:23 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "defines.hpp"
#include "StateMachine.hpp"

#define MACHINE_DEBUG 1

namespace webshell
{

bool validate_start(unsigned char c);
bool analyze_method(unsigned char c);
void generic_transition_function(void);
bool is_tchar(unsigned char c);

class MethodMachine
{
    public:
        MethodMachine();
        ~MethodMachine();

        void feed(unsigned char c);
        bool done(void);

    private:
        MethodMachine(const MethodMachine& other);
        MethodMachine& operator=(const MethodMachine& other);

        StateMachine<RequestLineState> _machine;
};

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

class VersionMachine
{
    public:
        VersionMachine();
        ~VersionMachine();

        void feed(unsigned char c);
        bool done(void);

    private:
        VersionMachine(const VersionMachine& other);
        VersionMachine& operator=(const VersionMachine& other);

        StateMachine<VersionState> _machine;
};

} // namespace webshell
