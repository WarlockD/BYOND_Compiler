/**********************************************
 * File: Expression.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the Expression class hierarchy.
 */

#include "Expression.hh"
#include <sstream>
#include <iterator>
#include <algorithm>
using namespace std;

/* CharacterClassExpression Implementation */
CharacterClassExpression::CharacterClassExpression(const set<char>& chars) :
	mChars(chars) {

}

set<char> CharacterClassExpression::characters() const {
	return mChars;
}

string CharacterClassExpression::toString() const {
	stringstream result;
	result << "(One of {";
	copy(mChars.begin(), mChars.end(), ostream_iterator<char>(result));
	result << "})";
	return result.str();
}

/* To convert a character class into an automaton, we build it like this:
 *
 *       (each char in the class)
 * START           -->            FINISH
 */
RegexpAutomaton CharacterClassExpression::toAutomaton(int exprNum, size_t priority) const {
	RegexpAutomaton result;
	result.start = new MachineState;
	result.terminal = new MachineState(exprNum, priority);

	/* Add connections. */
	for (set<char>::const_iterator itr = mChars.begin();
		itr != mChars.end(); ++itr)
		result.start->transitions.insert(make_pair(*itr, result.terminal));

	return result;
}




string ConcatenationExpression::toString() const {
	stringstream result;
	result << "(concat " << mLeft->toString()
		<< " " << mRight->toString() << ")";
	return result.str();
}

/* Our automaton looks like this:
 *        e                         e                          e
 * START -> LHS-START -?-> LHS-END --> RHS-START -?-> RHS-END --> END
 */
RegexpAutomaton ConcatenationExpression::toAutomaton(int exprNum, size_t priority) const {
	RegexpAutomaton result;
	result.start = new MachineState;
	result.terminal = new MachineState(exprNum, priority);

	RegexpAutomaton lhs = left()->toAutomaton(-1, numeric_limits<size_t>::max());
	RegexpAutomaton rhs = right()->toAutomaton(-1, numeric_limits<size_t>::max());

	/* Make their terminals no longer terminal. */
	lhs.terminal->terminalNumber = numeric_limits<int>::max();
	rhs.terminal->terminalNumber = numeric_limits<int>::max();

	/* Wire everything up. */
	result.start->eTransitions.insert(lhs.start);
	lhs.terminal->eTransitions.insert(rhs.start);
	rhs.terminal->eTransitions.insert(result.terminal);
	return result;
}



string DisjunctionExpression::toString() const {
	stringstream result;
	result << "(either " << mLeft->toString()
		<< " or " << mRight->toString() << ")";
	return result.str();
}

/* Our automaton looks like this:
 *          e
 *   START --> LHS-START -?-> LHS-END
 *   e |                         | e
 *     v                   e     V
 * RHS-START -?-> RHS-END -->   END
 */
RegexpAutomaton DisjunctionExpression::toAutomaton(int exprNum, size_t priority) const {
	RegexpAutomaton result;
	result.start = new MachineState;
	result.terminal = new MachineState(exprNum, priority);

	/* Get the child automata. */
	RegexpAutomaton lhs = left()->toAutomaton(-1, numeric_limits<size_t>::max()); // Will be overwritten
	RegexpAutomaton rhs = right()->toAutomaton(-1, numeric_limits<size_t>::max());

	/* Make their terminals no longer terminal. */
	lhs.terminal->terminalNumber = numeric_limits<int>::max();
	rhs.terminal->terminalNumber = numeric_limits<int>::max();

	/* Wire everything up! */
	result.start->eTransitions.insert(lhs.start);
	result.start->eTransitions.insert(rhs.start);
	lhs.terminal->eTransitions.insert(result.terminal);
	rhs.terminal->eTransitions.insert(result.terminal);

	return result;
}


string KleeneClosureExpression::toString() const {
	stringstream result;
	result << "(kleene closure of " << mExpr->toString() << ")";
	return result.str();
}

/* Our automaton looks like this:
 *                         e
 *               +-----------------+
 *        e      v                 |        e
 * START -->  EXPR-START  -?->  EXPR-END  -->  END
 *   |                                          ^
 *   +------------------------------------------+
 *                        e
 */
RegexpAutomaton KleeneClosureExpression::toAutomaton(int exprNum, size_t priority) const {
	RegexpAutomaton result;
	result.start = new MachineState;
	result.terminal = new MachineState(exprNum, priority);

	/* Grab nested automaton and make it no longer terminal. */
	RegexpAutomaton inner = expression()->toAutomaton(-1, numeric_limits<size_t>::max());
	inner.terminal->terminalNumber = numeric_limits<int>::max();

	/* Add connections. */
	result.start->eTransitions.insert(result.terminal);
	result.start->eTransitions.insert(inner.start);
	inner.terminal->eTransitions.insert(inner.start);
	inner.terminal->eTransitions.insert(result.terminal);

	return result;
}

/* Empty string expression. */
string EmptyStringExpression::toString() const {
	return "(empty string)";
}

/* To convert a character class into an automaton, we build it like this:
 *
 *         e
 * START  -->  FINISH
 */
RegexpAutomaton EmptyStringExpression::toAutomaton(int exprNum, size_t priority) const {
	RegexpAutomaton result;
	result.start = new MachineState;
	result.terminal = new MachineState(exprNum, priority);
	result.start->eTransitions.insert(result.terminal);
	return result;
}