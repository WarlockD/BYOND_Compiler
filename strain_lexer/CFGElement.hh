/********************************************
 * File: CFGElement.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Symbols for the context-free grammar parsed by
 * the LR(1) parser.  The context-free grammar for
 * expressions is defined as follows:
 *
 * R' ->  R
 * R  ->  S
 * R  ->  SR
 * S  ->  T
 * S  ->  T|S
 * T  ->  U
 * T  ->  U*
 * U  ->  ch
 * U  ->  (R)
 *
 * Here, ch is a wildcard for any character, character class,
 * or character class expression.  To simplify the parser,
 * we parse these expressions separately from the main
 * parser.  This means that the resulting CFG elements are
 * R S T U ( ) | * ch $
 */

#ifndef CFGElement_Included
#define CFGElement_Included

#include "Lexer.hh"
#include "Expression.hh"


/* Enumerated type defining the CFG elements. */
enum CFGElement {
	OPEN_PAREN_ELEM = strain::Lexer::Lexeme_OpenParenthesis,
	CLOSE_PAREN_ELEM = strain::Lexer::Lexeme_CloseParenthesis,
	OR_ELEM = strain::Lexer::Lexeme_Disjunction,
	STAR_ELEM = strain::Lexer::Lexeme_KleeneClosure,
	EOF_ELEM = strain::Lexer::None,
	CHARACTER_ELEM,
	R_ELEM,
	S_ELEM,
	T_ELEM,
	U_ELEM,
	START_ELEM // Starting element; not really a CFG symbol
};

/* Struct holding a triple of a CFG element, a state,
 * and an optional expression.
 */
struct StackElement {
	CFGElement type;
	int state;
	expr_t expr;
#if 0
	StackElement(CFGElement type, expr_t expr, int state = 0) {
		this->type = type;
		this->state = state;
		this->expr = expr;
	}

	template<typename T>
	StackElement(CFGElement type, T* expr, int state = 0) {
		this->type = type;
		this->state = state;
		this->expr = expr->shared_from_this();
	}
#endif
	StackElement(CFGElement type, int state = 0) {
		this->type = type;
		this->state = state;
		this->expr = nullptr;
	}
	StackElement(CFGElement type, expr_t expr, int state = 0) :type(type), expr(expr), state(state) {}
	StackElement(CFGElement type, Expression* expr, int state = 0) :type(type), expr(expr->shared_from_this()), state(state) {}
};
	 


#endif