/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StateMachine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 19:27:27 by mhuszar           #+#    #+#             */
/*   Updated: 2024/10/23 20:46:49 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <vector>
#include <map>
#include <exception>
#include <set>

template<class TState>
class StateMachine
{
    typedef void (*transitionFunction)();
    typedef bool (*actionFunction)(unsigned char c);
    typedef std::map<TState, transitionFunction> t_options;

    public:
        
        StateMachine();
        ~StateMachine();
        
        void addState(const TState& state);
        void addTransition(const TState& startState, const TState& finalState, transitionFunction lambda);
        void addAction(const TState& state, actionFunction lambda);
        void transitionTo(const TState& state);
        bool update(unsigned char c);
        TState getCurrentState() const;

    private:

        StateMachine(const StateMachine& other);
        StateMachine& operator=(const StateMachine& other);
        
        TState                              _current_state;
        std::set<TState>                    _states;
        std::map<TState, t_options>         _transitions;
        std::map<TState, actionFunction>    _actions;
        bool                                _setup;
};

#include "StateMachine.tpp"

#endif