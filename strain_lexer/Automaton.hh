/************************************************
 * File: Automaton.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A set of structs defining DFAs, NFAs, and associated
 * operations on them.
 */

#ifndef Automaton_Included
#define Automaton_Included

#include <set>
#include <map>
#include <limits>

 /* A struct representing a state in an e-NFA.  Because the lexer we will
  * be generating has multiple different types of terminal states (one for
  * each type of token, at least), we tag each state with a terminal number
  * rather than a terminal flag.  numeric_limits<int>::max() indicates that
  * the state is not a terminal.
  */
struct MachineState {
	std::multimap<char, MachineState*> transitions; // Transition table
	std::set<MachineState*> eTransitions; // Set of e-transitions.

	int terminalNumber; // std::numeric_limits<int>::max() if not terminal.
						// Otherwise, the number of the expression this is
						// a terminal for.

	size_t priority;    // The priority associated with this state.  If the
						// state is terminal, this is used to determine which
						// of a family of states wins.  Smaller values
						// indicate higher priorities.

	/* Convenience ctor */
	explicit MachineState(int terminal = std::numeric_limits<int>::max(),
		int priority = std::numeric_limits<size_t>::max()) {
		this->terminalNumber = terminal;
		this->priority = priority;
	}
};

/* A struct representing a restricted e-NFA with a
 * single transition leaving its start state and
 * a single terminal state.  This is used to generate
 * complex regexp automatons inductively on regular
 * expressions.
 *
 * If terminal is null, then this represents an
 * ordinary automaton that does not have a single
 * dedicated terminal state.
 */
struct RegexpAutomaton {
	MachineState* start;       // Dedicated start state
	MachineState* terminal;    // Dedicated terminal state

	/* Convenience ctor. */
	RegexpAutomaton() {
		start = terminal = 0;
	}
};

/**
 * Given an e-NFA, performs the subset construction to convert it to
 * a regular DFA.
 */
RegexpAutomaton DoSubsetConstruction(RegexpAutomaton automaton);

/**
 * Given a DFA, returns a minimum-state DFA equivalent to the
 * original DFA.
 */
void MinimizeDFA(RegexpAutomaton& automaton);

#endif