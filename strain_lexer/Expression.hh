/****************************************************
 * File: Expression.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A class hierarchy of expressions.  These classes are
 * essentially the AST from which we can derive a
 * state machine representation of the regular expression.
 */

#ifndef Expression_Included
#define Expression_Included

#include <string>
#include <set>
#include <memory>
#include "Automaton.hh"

 /* Base class of all expressions. */
class Expression : public std::enable_shared_from_this<Expression> {
public:
	virtual ~Expression() {}

	/* Provides a string representation of the expression. */
	virtual std::string toString() const = 0;

	/* Converts the expression to an automaton where each terminal state
	 * has the specified value and priority.
	 */
	virtual RegexpAutomaton toAutomaton(int exprNum, size_t priority) const = 0;
};
using expr_t = std::shared_ptr<Expression>;

/* Character class expression. */
class CharacterClassExpression : public Expression {
public:
	CharacterClassExpression() {}
	explicit CharacterClassExpression(const std::set<char>& chars);
	std::string toString() const;
	RegexpAutomaton toAutomaton(int exprNum, size_t priority) const;

	/* The set of characters represented by this expression.  This can be
	 * zero, one, or many characters depending on what created this.
	 */
	std::set<char> characters() const;

private:
	const std::set<char> mChars;
};

/* Concatenation expression. */
class ConcatenationExpression : public Expression {
public:
	ConcatenationExpression(expr_t left, expr_t right) : mLeft(left), mRight(right) {}
	ConcatenationExpression(Expression* left, Expression* right) : mLeft(left), mRight(right) {}
	std::string toString() const;
	RegexpAutomaton toAutomaton(int exprNum, size_t priority) const;

	/* The left and right subexpressions. */
	Expression* left() const { return mLeft.get(); }
	Expression* right() const { return mRight.get(); }

private:
	expr_t  mLeft;
	expr_t  mRight;
};

/* Disjunction expression. */
class DisjunctionExpression : public Expression {
public:
	DisjunctionExpression(expr_t left, expr_t right) : mLeft(left), mRight(right) {}
	DisjunctionExpression(Expression* left, Expression* right) : mLeft(left), mRight(right) {}
	std::string toString() const;
	RegexpAutomaton toAutomaton(int exprNum, size_t priority) const;

	/* The left and right subexpressions. */
	Expression* left() const { return mLeft.get(); }
	Expression* right() const { return mRight.get(); }

private:
	expr_t  mLeft;
	expr_t  mRight;
};

/* Kleene closure expression. */
class KleeneClosureExpression : public Expression {
public:
	explicit KleeneClosureExpression(expr_t expr) : mExpr(expr) {}
	explicit KleeneClosureExpression(Expression* expr) : mExpr(expr) {}
	std::string toString() const;
	RegexpAutomaton toAutomaton(int exprNum, size_t priority) const;

	/* The expression to be iterated. */
	Expression* expression() const { return mExpr.get(); }

private:
	expr_t mExpr;
};

/* Empty expression. */
class EmptyStringExpression : public Expression {
public:
	std::string toString() const;
	RegexpAutomaton toAutomaton(int exprNum, size_t priority) const;
};

#endif