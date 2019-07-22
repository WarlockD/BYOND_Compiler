/***********************************************************
 * File: Parser.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Code to parse a sequence of lexemes into a RegexpAutomaton.
 *
 * This parser is hand-written and, unfortunately, is not the
 * most elegant parsing code.  If I get a chance to rewrite this,
 * I will certainly do so.  Even better, if I ever getting around
 * to writing a parser generator, I'll use it to generate this
 * code for me.
 *
 * The main grammar we use for regular expressions is as follows:
 *
 * R -> S
 * R -> SR
 * S -> T
 * S -> T|S
 * T -> U
 * T -> U*
 * U -> C
 * U -> (R)
 * C -> ch       For any single character ch
 * C -> [class]  For any character class class
 * C -> {E}
 * C -> ~
 * E -> C
 * E -> E + C
 * E -> E - C
 *
 * Here:
 * R is a complete regular expression.
 * S is a regular expression, minus concatenation
 * T is a regular expression, minus disjunction
 * U is a regular expression, minus Kleene closure
 * C is a set of characters.
 * E is a set of union and subtractions of sets of characters.
 *
 * The problem with this regular expression grammar is twofold.  First,
 * it is not SLR, nor is it LALR(1).  Consequently, we use an LR(1)
 * parser, which is considerably more complex.  Since I had to construct
 * this parser by hand, I ended up simplifying the grammar down by having
 * a two-pass parser.  In the first pass, all character expressions are
 * reduced down to a single character lexeme by using a simple recursive
 * descent parser.  In the second pass, we use an LR(1) parser for this
 * grammar:
 *
 * R -> S
 * R -> SR
 * S -> T
 * S -> T|S
 * T -> U
 * T -> U*
 * U -> c
 * U -> (R)
 *
 * Where c is any character or character class expression.
 *
 * One problem with this grammar which I did not notice until after I had
 * written and coded the LR(1) parser is that it disallows consecutive
 * Kleene stars, so the regexp a***** is invalid.  As a hacky fix, I
 * changed the lexer so that it compresses consecutive stars into a single
 * star.  This is a really egregious hack, and when/if I get around to
 * rewriting this code I will certainly fix it.
 */



#include <typeinfo>
#include <iostream>
#include <set>
#include <algorithm>
#include <stdexcept>


#include "Parser.hh"
#include "Expression.hh"
#include "CFGElement.hh"
#include "LR1Parse.hh"

#include "Lexer.hh"

using namespace std;
using namespace strain;

/* Functions for compressing character expressions into single lexemes. */
CharacterClassExpression* ReadSingleCharacter(Lexer& lexemes);
CharacterClassExpression* ReadCharacterClass(Lexer& lexemes);


/* Given a LexemeQueue, preprocesses the tokens it contains into
 * a stream of CFG elements.
 */
void PreprocessStream(Lexer& lexemes, CFGQueue& tokens) {
	/* Since we're always guaranteed to have at least one lexeme
	 * (namely EOF), we implement this as a do...while loop rather
	 * than a while loop.
	 */
	pair<Lexer::LexemeType, Lexeme> lexeme;
	do {
		lexeme = lexemes.front();

		/* If the lexeme is something we could potentially read
		 * as a character expression, compress that entire expression
		 * down into a single CharacterClassExpression.
		 */
		if (lexeme.first == Lexer::Lexeme_Character ||
			lexeme.first == Lexer::Lexeme_CharacterClass ||
			lexeme.first == Lexer::Lexeme_OpenCurlyBrace ||
			lexeme.first == Lexer::Lexeme_EmptyString) {
			CharacterClassExpression* charExpr = ReadSingleCharacter(lexemes);
			tokens.push(StackElement(CHARACTER_ELEM, charExpr));
		}
		/* Otherwise, convert from the corresponding lexemes to the
		 * corresponding CFG element.
		 */
		else {
			/* We need to dequeue the element ourselves since
			 * otherwise it won't happen.
			 */
			lexemes.next();

			switch (lexeme.first) {
			case Lexer::Lexeme_OpenParenthesis:
			case Lexer::Lexeme_CloseParenthesis:
			case Lexer::Lexeme_KleeneClosure:
			case Lexer::Lexeme_Disjunction:
				tokens.push(StackElement(CFGElement(lexeme.first),
					lexeme.second.get<expr_t>()));
				break;
			case Lexer::None:
				tokens.push(StackElement(CFGElement(Lexer::None)));
				break;
			default:
				throw invalid_argument("Parse error during preprocessing.");
			}
		}
	} while (lexeme.first != Lexer::None);
}

CharacterClassExpression Expression;
/* Parses a stream of lexemes into a single Expression.
 *
 * This parser works in two phases.  In the first phase,
 * the lexemes are scanned and all character expressions
 * (single characters, character classes, and character
 * expressions) are replaced with single CFG tokens.  In
 * the second, the token stream is parsed using a
 * canonical LR(1) parser.
 */
expr_t Parse(Lexer& lexemes) {
	/* Convert from a queue of lexemes to a queue of pairs of
	 * CFG elements and parser states.  This also replaces
	 * character expressions with single character symbols.
	 */
	CFGQueue tokens;
	PreprocessStream(lexemes, tokens);
	return LR1Parse(tokens);
}

/* Reads a part of a regexp corresponding to a single character. */
CharacterClassExpression* ReadSingleCharacter(Lexer& lexemes) {
	/* See what this is.  If it's a single character or a character
	 * class, hand it back as-is.
	 */
	pair<Lexer::LexemeType, Lexeme> lex = lexemes.next();
	if (lex.first == Lexer::Lexeme_Character ||
		lex.first == Lexer::Lexeme_CharacterClass ||
		lex.first == Lexer::Lexeme_EmptyString)
		return dynamic_cast<CharacterClassExpression*>(lex.second.get<expr_t>().get());   

	/* If it's a curly brace, it's a list. */
	else if (lex.first == Lexer::Lexeme_OpenCurlyBrace) {
		CharacterClassExpression* charList = ReadCharacterClass(lexemes);
		if (lexemes.next().first != Lexer::Lexeme_CloseCurlyBrace)
			throw invalid_argument("Parse error.");
		return charList;
	}
	else throw invalid_argument("Parse error.");
}

/* Continuously pulls more and more single character expressions in,
 * terminating when there are no more symbols to read.  Because we want
 * this grammar to be left-associative (i.e. a + b - c should be
 * (a + b) - c and not a + (b - c), the logic is a bit tricky.
 */
CharacterClassExpression* ReadCharacterClass(Lexer& lexemes) {
	/* First, read a character class out. */
	CharacterClassExpression* accumulator = ReadSingleCharacter(lexemes);

	/* Now, keep scanning the rest of the input until we hit a close
	 * brace.
	 */
	while (lexemes.front().first != Lexer::Lexeme_CloseCurlyBrace) {
		pair<Lexer::LexemeType, Lexeme> lexeme = lexemes.next();

		/* If we get a plus, this is character class union. */
		if (lexeme.first == Lexer::Lexeme_Union) {
			CharacterClassExpression* rhs = ReadSingleCharacter(lexemes);
			set<char> resultSet, lhsSet = accumulator->characters(),
				rhsSet = rhs->characters();

			/* Compute their union and wrap it up. */
			set_union(lhsSet.begin(), lhsSet.end(),
				rhsSet.begin(), rhsSet.end(),
				inserter(resultSet, resultSet.begin()));
			accumulator = new CharacterClassExpression(resultSet);
		}

		/* If we get a minus, this is a character class difference. */
		else if (lexeme.first == Lexer::Lexeme_Subtraction) {
			CharacterClassExpression* rhs = ReadSingleCharacter(lexemes);
			set<char> resultSet, lhsSet = accumulator->characters(),
				rhsSet = rhs->characters();
			set_difference(lhsSet.begin(), lhsSet.end(),
				rhsSet.begin(), rhsSet.end(),
				inserter(resultSet, resultSet.begin()));
			accumulator = new CharacterClassExpression(resultSet);
		}
		else
			throw invalid_argument("Parse error");
	}
	return accumulator;
}
