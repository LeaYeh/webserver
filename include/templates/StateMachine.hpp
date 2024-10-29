/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StateMachine.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhuszar <mhuszar@student.42vienna.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 19:27:27 by mhuszar           #+#    #+#             */
/*   Updated: 2024/10/31 22:46:33 by mhuszar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <map>
#include <set>

template <class TState> class StateMachine
{
    typedef void (*transitionFunction)();
    typedef bool (*actionFunction)(unsigned char c);
    typedef std::map<TState, transitionFunction> t_options;

  public:
    StateMachine();
    ~StateMachine();

    void addState(const TState& state);
    void addTransition(const TState& startState, const TState& finalState,
                       transitionFunction lambda);
    void addAction(const TState& state, actionFunction lambda);
    void transitionTo(const TState& state);
    void setFinalState(const TState& state);
    bool done(void);
    bool feed(unsigned char c);
    TState getCurrentState(void) const;
    // virtual void SetupTransitions(void);

  private:
    StateMachine(const StateMachine& other);
    StateMachine& operator=(const StateMachine& other);

  protected:
    // unsigned char _ch;
    TState _current_state;
    TState _final_state;
    std::set<TState> _states;
    std::map<TState, t_options> _transitions;
    std::map<TState, actionFunction> _actions;
    bool _setup;

    virtual void _validate_start(unsigned char c);
    virtual void _analyze_method(unsigned char c);
};

#include "StateMachine.tpp"

#endif
