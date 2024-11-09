/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StateMachine.tpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 19:31:24 by mhuszar           #+#    #+#             */
/*   Updated: 2024/11/06 16:49:48 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <stdexcept>
#include "ParseException.hpp"

template <class TState> StateMachine<TState>::StateMachine()
{
    _setup = false;
}

template <class TState> StateMachine<TState>::~StateMachine()
{
}

template <class TState> void StateMachine<TState>::addState(const TState& state)
{
    if (!_setup)
    {
        _current_state = state;
        _setup = true;
    }
    _states.insert(state);
}

template <class TState>
void StateMachine<TState>::addTransition(const TState& startState,
                                         const TState& finalState,
                                         transitionFunction lambda)
{
    if (_states.find(startState) == _states.end() ||
        _states.find(finalState) == _states.end())
    {
        throw std::runtime_error("Setup error: state does not exist");
    }
    std::pair<TState, transitionFunction> new_option;
    new_option.first = finalState;
    new_option.second = lambda;
    typename std::map<TState, t_options>::iterator iter =
        _transitions.find(startState);
    if (iter != _transitions.end())
    {
        _transitions[startState].insert(new_option);
    }
    else
    {
        t_options new_option_map;
        new_option_map.insert(new_option);
        std::pair<TState, t_options> new_transition;
        new_transition.first = startState;
        new_transition.second = new_option_map;
        _transitions.insert(new_transition);
    }
}

// template <class TState>
// void StateMachine<TState>::SetupTransitions(void)
// {
    
// }

template <class TState>
void StateMachine<TState>::addAction(const TState& state, actionFunction lambda)
{
    if (_states.find(state) == _states.end())
    {
        throw std::runtime_error("Setup error: state does not exist");
    }
    if (_actions.find(state) != _actions.end())
    {
        throw std::runtime_error("Setup error: action already has a state");
    }
    std::pair<TState, actionFunction> new_action;
    new_action.first = state;
    new_action.second = lambda;
    _actions.insert(new_action);
}

template <class TState>
void StateMachine<TState>::transitionTo(const TState& state)
{
    if (_states.find(state) == _states.end())
    {
        throw std::runtime_error("Transition error: state does not exist");
    }
    if (_transitions.find(_current_state) == _transitions.end())
    {
        throw std::runtime_error(
            "Transition error: no transition for current state");
    }
    else if (_transitions[_current_state].find(state) ==
             _transitions[_current_state].end())
    {
        throw std::runtime_error("Transition error: no transition from current "
                                 "state to target state");
    }
    _transitions[_current_state][state]();
    _current_state =
        state; // i cant put this before the previous line lol im stupid
}

template <class TState>
bool StateMachine<TState>::feed(unsigned char c)
{
    if (_actions.find(_current_state) == _actions.end())
    {
        std::cerr << "Map size: " << _actions.size() << std::endl;
        std::cerr << "Right pos holds: " << _actions.end()->first;
        std::cerr << "For state: " << _current_state << " char: " << c << " : " << std::endl;
        throw std::runtime_error("Action error: no action for current state");
    }
    return (_actions[_current_state](c));
}

template <class TState>
TState StateMachine<TState>::getCurrentState() const
{
    return (_current_state);
}

template <class TState>
void StateMachine<TState>::setCurrentState(TState state)
{
    _current_state = state;
}

template <class TState>
void StateMachine<TState>::setFinalState(const TState& state)
{
    _final_state = state;
}

template <class TState>
bool StateMachine<TState>::done(void)
{
    return (_current_state == _final_state);
}

template <class TState>
StateMachine<TState>::StateMachine(const StateMachine& other) : _current_state(other._current_state),
    _final_state(other._final_state), _states(other._states), _transitions(other._transitions),
    _actions(other._actions), _setup(other._setup)
{
}

template <class TState>
StateMachine<TState>& StateMachine<TState>::operator=(const StateMachine& other)
{
    if (this == &other)
        return (*this);
    _current_state = other._current_state;
    _final_state = other._final_state;
    _states = other._states;
    _transitions = other._transitions;
    _actions = other._actions;
    _setup = other._setup;
    return (*this);
}

template <class TState>
void StateMachine<TState>::_analyze_method(unsigned char c)
{
    (void)c;
    // if (c == ' ')
    //     transitionTo(SPACE_BEFORE_URI);
    // else if (!is_tchar(c))
    //     throw (ParseException(BAD_REQUEST, BAD_REQUEST_MSG));
}

template <class TState>
void StateMachine<TState>::_validate_start(unsigned char c)
{
    (void)c;
    // if (!is_tchar(c))
    //     throw (ParseException(BAD_REQUEST, BAD_REQUEST_MSG));
    // else
    //     transitionTo(METHOD);
}