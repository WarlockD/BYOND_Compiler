/*************************************************
 * File: LR1Parse.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Exports an LR(1) parser to convert a modified
 * token stream into an Expression.  See the
 * .cpp file for a discussion of the actual
 * parser.
 */

#ifndef LR1Parse_Included
#define LR1Parse_Included

#include "CFGElement.hh"
#include <queue>


class Expression;
typedef std::queue<StackElement> CFGQueue;

/* Uses an LR(1) parser to parse a CFG queue. */
expr_t LR1Parse(CFGQueue& queue);

#endif