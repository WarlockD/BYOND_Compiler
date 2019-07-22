/*************************************************
 * File: CodeGenerator.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * An interface representing something that can
 * generate code for the lexer.  There is currently
 * only one of these, though I hope to someday
 * extend the hierarchy to handle PHP, Java, etc.
 */
#ifndef CodeGenerator_Included
#define CodeGenerator_Included

#include <vector>
#include <string>
#include <utility>
#include <ostream>
#include "StrainFile.hh"
struct RegexpAutomaton;

class CodeGenerator {
public:
	virtual ~CodeGenerator() {}

	/* The parameters are:
	 * 1. The minimized DFA automaton.
	 * 2. The source file.
	 */
	virtual void writeLexer(RegexpAutomaton automaton,
		const StrainFile& file) const = 0;
};

/* A class that builds up a C++ lexer. */
class CCodeGenerator : public CodeGenerator {
	virtual void writeLexer(RegexpAutomaton automaton,
		const StrainFile& tokens) const;
};

#endif