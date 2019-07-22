/*************************************************
 * File: Parser.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Exports the Parse function, which converts a
 * token stream into an Expression.
 */

#ifndef Parser_Included
#define Parser_Included

#include "Lexer.hh"

 /* Forward declarations */
class Expression;

/**
 * Parses the specified LexemeQueue into a regular expression.
 */
expr_t Parse(strain::Lexer& lexemes);

#endif
