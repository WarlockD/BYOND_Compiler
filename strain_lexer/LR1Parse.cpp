/************************************************
 * File: LR1Parse.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * An LR(1) parser that can read the following
 * grammar:
 *
 * R -> S    (0)
 * R -> SR   (1)
 * S -> T    (2)
 * S -> T|S  (3)
 * T -> U    (40
 * T -> U*   (5)
 * U -> c    (6)
 * U -> (R)  (7)
 *
 * The full parse table for this parser can be found
 * in the file parser.txt.  This code is simply an
 * implementation of that parser.
 */

#include "LR1Parse.hh"
#include "Expression.hh"
#include <cassert>
#include <stdexcept>
#include <stack>
#include <iostream>
#include <iomanip>
using namespace std;

/* Shifts a symbol onto the stack. */
void Shift(stack<StackElement>&, CFGQueue&, StackElement&, int nextState);

/* Reduces a set of symbols atop the stack. */
void Reduce(stack<StackElement>&, CFGQueue&, StackElement&, int rType);

/* Reports a parse error. */
void Fail();

/* For logging purposes, prints out the state of the stack. */
void PrintStack(stack<StackElement> s) {
	/* Get a copy of the stack in reverse order. */
	stack<StackElement> reverse;
	while (!s.empty()) {
		reverse.push(s.top());
		s.pop();
	}

	/* Print out the stack one symbol/state pair at a time. */
	clog << "Stack: ";
	while (!reverse.empty()) {
		StackElement top = reverse.top();
		reverse.pop();

		switch (top.type) {
		case R_ELEM:
			clog << "R";
			break;
		case T_ELEM:
			clog << "T";
			break;
		case S_ELEM:
			clog << "S";
			break;
		case U_ELEM:
			clog << "U";
			break;
		case STAR_ELEM:
			clog << "*";
			break;
		case OPEN_PAREN_ELEM:
			clog << "(";
			break;
		case CLOSE_PAREN_ELEM:
			clog << ")";
			break;
		case OR_ELEM:
			clog << "|";
			break;
		case EOF_ELEM:
			clog << "$";
			break;
		case CHARACTER_ELEM:
			clog << "a";
			break;
		default:
			clog << "?";
			break;
		}

		clog << setfill('0') << setw(2) << top.state << " ";
	}
	clog << endl;
}

/* Does an LR1 parse of the specified queue.  The shift table is encoded
 * in this function; the code for reductions is in Reduce.
 */
expr_t LR1Parse(CFGQueue& queue) {
	/* Parse stack. */
	stack<StackElement> stack;
	stack.push(StackElement(START_ELEM)); // State zero, no expression.

	/* The next symbol. */
	StackElement next = queue.front();
	queue.pop();

	/* Do the parse! */
	while (true) {
		PrintStack(stack);

		switch (stack.top().state) {
		case 0:
			if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 6);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 7);
			else
				Fail();
			break;
		case 1:
			/* Accept! */
			if (next.type == EOF_ELEM)
				return stack.top().expr;
			else
				Fail();
			break;
		case 2:
			if (next.type == EOF_ELEM)
				Reduce(stack, queue, next, 0);
			else if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 6);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 7);
			else
				Fail();
			break;
		case 3:
			if (next.type == EOF_ELEM)
				Reduce(stack, queue, next, 1);
			else
				Fail();
			break;
		case 4:
			if (next.type == OR_ELEM)
				Shift(stack, queue, next, 8);
			else if (next.type == EOF_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM)
				Reduce(stack, queue, next, 2);
			else
				Fail();
			break;
		case 5:
			if (next.type == STAR_ELEM)
				Shift(stack, queue, next, 9);
			else if (next.type == EOF_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM ||
				next.type == OR_ELEM)
				Reduce(stack, queue, next, 4);
			else
				Fail();
			break;
		case 6:
			if (next.type != CLOSE_PAREN_ELEM)
				Reduce(stack, queue, next, 6);
			else
				Fail();
			break;
		case 7:
			if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 15);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 19);
			else
				Fail();
			break;
		case 8:
			if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 6);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 7);
			else
				Fail();
			break;
		case 9:
			if (next.type != CLOSE_PAREN_ELEM &&
				next.type != STAR_ELEM)
				Reduce(stack, queue, next, 5);
			else
				Fail();
			break;
		case 10:
			if (next.type == CLOSE_PAREN_ELEM)
				Shift(stack, queue, next, 11);
			else
				Fail();
			break;
		case 11:
			if (next.type != CLOSE_PAREN_ELEM)
				Reduce(stack, queue, next, 7);
			else
				Fail();
			break;
		case 12:
			if (next.type == CLOSE_PAREN_ELEM)
				Reduce(stack, queue, next, 0);
			else if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 15);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 19);
			else
				Fail();
			break;
		case 13:
			if (next.type == CLOSE_PAREN_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM)
				Reduce(stack, queue, next, 2);
			else if (next.type == OR_ELEM)
				Shift(stack, queue, next, 20);
			else
				Fail();
			break;
		case 14:
			if (next.type == CLOSE_PAREN_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM ||
				next.type == OR_ELEM)
				Reduce(stack, queue, next, 4);
			else if (next.type == STAR_ELEM)
				Shift(stack, queue, next, 21);
			else
				Fail();
			break;
		case 15:
			if (next.type != EOF_ELEM)
				Reduce(stack, queue, next, 6);
			else
				Fail();
			break;
		case 16:
			if (next.type == EOF_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM)
				Reduce(stack, queue, next, 3);
			else
				Fail();
			break;
		case 17:
			if (next.type == EOF_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM ||
				next.type == OR_ELEM)
				Reduce(stack, queue, next, 4);
			else if (next.type == STAR_ELEM)
				Shift(stack, queue, next, 9);
			else
				Fail();
			break;
		case 18:
			if (next.type == CLOSE_PAREN_ELEM)
				Reduce(stack, queue, next, 1);
			else
				Fail();
			break;
		case 19:
			if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 15);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 19);
			else
				Fail();
			break;
		case 20:
			if (next.type == CHARACTER_ELEM)
				Shift(stack, queue, next, 15);
			else if (next.type == OPEN_PAREN_ELEM)
				Shift(stack, queue, next, 19);
			else
				Fail();
			break;
		case 21:
			if (next.type == CLOSE_PAREN_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM ||
				next.type == OR_ELEM)
				Reduce(stack, queue, next, 5);
			else
				Fail();
			break;
		case 22:
			if (next.type == CLOSE_PAREN_ELEM)
				Shift(stack, queue, next, 23);
			else
				Fail();
			break;
		case 23:
			if (next.type == CLOSE_PAREN_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM ||
				next.type == OR_ELEM ||
				next.type == STAR_ELEM)
				Reduce(stack, queue, next, 7);
			else
				Fail();
			break;
		case 24:
			if (next.type == CLOSE_PAREN_ELEM ||
				next.type == CHARACTER_ELEM ||
				next.type == OPEN_PAREN_ELEM)
				Reduce(stack, queue, next, 3);
			else
				Fail();
			break;
		}
	}
}

/* Shifts a new symbol and state. */
void Shift(stack<StackElement>& stack, CFGQueue& queue,
	StackElement& next, int nextState) {
	/* Push the current state onto the stack. */
	stack.push(next);

	/* Update state. */
	stack.top().state = nextState;

	/* Pull from the queue into the next element slot. */
	next = queue.front();
	queue.pop();
}

/* Helper method which does a top/pop pair on a stack as one function. */
template <typename T>
T Pop(stack<T>& stack) {
	T result = stack.top();
	stack.pop();
	return result;
}

/* Grabs symbols off the stack and does a reduction.  Returns
 * a new StackElement updated with the correct symbol.
 */
StackElement DoReduction(stack<StackElement>& stack, int rType) {
	switch (rType) {
	case 0: { // R -> S 
		StackElement top = Pop(stack);
		top.type = R_ELEM;
		return top;
	}
	case 1: { // R -> SR
		StackElement r = Pop(stack);
		StackElement s = Pop(stack);
		return StackElement(R_ELEM, std::make_shared<Expression>(new ConcatenationExpression(s.expr, r.expr)));
	}
	case 2: { // S -> T
		StackElement top = Pop(stack);
		top.type = S_ELEM;
		return top;
	}
	case 3: { // S -> T | S
		StackElement s = Pop(stack);
		StackElement bar = Pop(stack);
		StackElement t = Pop(stack);
		return StackElement(S_ELEM, new DisjunctionExpression(t.expr, s.expr));
	}
	case 4: { // T -> U
		StackElement top = Pop(stack);
		top.type = T_ELEM;
		return top;
	}
	case 5: { // T -> U*
		StackElement star = Pop(stack);
		StackElement u = Pop(stack);
		return StackElement(T_ELEM, new KleeneClosureExpression(u.expr));
	}
	case 6: { // U -> c
		StackElement top = Pop(stack);
		top.type = U_ELEM;
		return top;
	}
	case 7: { // U -> (R)
		StackElement closeParen = Pop(stack);
		StackElement r = Pop(stack);
		StackElement openParen = Pop(stack);
		r.type = U_ELEM;
		return r;
	}
	default:
		Fail();
		throw logic_error("Failed, but kept going?");
	}
}

/* Does a reduction.  This is tricky because we have to both perform the
 * reduction and fire the goto table.
 */
void Reduce(stack<StackElement>& stack, CFGQueue& queue,
	StackElement& next, int rType) {
	/* Handle the reduction. */
	StackElement result = DoReduction(stack, rType);

	/* Fire the goto table. */
	switch (stack.top().state) {
	case 0:
		if (result.type == R_ELEM)
			result.state = 1;
		else if (result.type == S_ELEM)
			result.state = 2;
		else if (result.type == T_ELEM)
			result.state = 4;
		else if (result.type == U_ELEM)
			result.state = 5;
		else
			Fail();
		break;
	case 1:
		Fail(); // Shouldn't be reducing here!
		break;
	case 2:
		if (result.type == R_ELEM)
			result.state = 3;
		else if (result.type == S_ELEM)
			result.state = 2;
		else if (result.type == T_ELEM)
			result.state = 4;
		else if (result.type == U_ELEM)
			result.state = 5;
		else
			Fail();
		break;
	case 3:
		Fail(); // Can't reduce to here.
		break;
	case 4:
		Fail(); // Can't reduce here.
		break;
	case 5:
		Fail();
		break;
	case 6:
		Fail();
		break;
	case 7:
		if (result.type == R_ELEM)
			result.state = 10;
		else if (result.type == S_ELEM)
			result.state = 12;
		else if (result.type == T_ELEM)
			result.state = 13;
		else if (result.type == U_ELEM)
			result.state = 14;
		else
			Fail();
		break;
	case 8:
		if (result.type == S_ELEM)
			result.state = 16;
		else if (result.type == T_ELEM)
			result.state = 4;
		else if (result.type == U_ELEM)
			result.state = 17;
		else
			Fail();
		break;
	case 9:
		Fail();
		break;
	case 10:
		Fail();
		break;
	case 11:
		Fail();
		break;
	case 12:
		if (result.type == R_ELEM)
			result.state = 18;
		else if (result.type == S_ELEM)
			result.state = 12;
		else if (result.type == T_ELEM)
			result.state = 13;
		else if (result.type == U_ELEM)
			result.state = 14;
		else
			Fail();
		break;
	case 13:
		Fail();
		break;
	case 14:
		Fail();
		break;
	case 15:
		Fail();
		break;
	case 16:
		Fail();
		break;
	case 17:
		Fail();
		break;
	case 18:
		Fail();
		break;
	case 19:
		if (result.type == R_ELEM)
			result.state = 22;
		else if (result.type == S_ELEM)
			result.state = 12;
		else if (result.type == T_ELEM)
			result.state = 13;
		else if (result.type == U_ELEM)
			result.state = 14;
		else
			Fail();
		break;
	case 20:
		if (result.type == S_ELEM)
			result.state = 24;
		else if (result.type == T_ELEM)
			result.state = 13;
		else if (result.type == U_ELEM)
			result.state = 14;
		else
			Fail();
		break;
	case 21:
		Fail();
		break;
	case 22:
		Fail();
		break;
	case 23:
		Fail();
		break;
	case 24:
		Fail();
		break;
	}

	/* Put the next element on top of the stack. */
	stack.push(result);
}

/* Reports an error both by asserting and by throwing an exception. */
void Fail() {
	assert(false);
	throw invalid_argument("Parse error.");
}