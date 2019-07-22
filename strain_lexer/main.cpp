/************************************************
 * File: main.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Main entry point for the Strain lexer.  This
 * code glues together all of the pieces defined
 * elsewhere into a coherent executable.
 */

#include "Lexer.hh"
#include "Parser.hh"
#include "Expression.hh"
#include "CFGElement.hh"
#include "Automaton.hh"
#include "CodeGenerator.hh"
#include <iostream>
#include <set>
#include <queue>
#include <iomanip>
#include <typeinfo>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <sstream>
using namespace std;
using namespace strain;

/* Configuration struct.  This holds information about what
 * operation we are to perform.
 */
struct StrainConfiguration {
	bool isVerbose;           // Print lots of output?
	istream* inputFile;         // Name of the input file.
	CodeGenerator* generator; // The code generator to use on the result.
};

/* Prints info about how this is supposed to be used. */
void PrintUsage() {
	cout << "strain: Like flex, but more likely to hurt." << endl;
	cout << "Usage: strain inputfile [-v]" << endl;
	cout << "   -v: Verbose mode." << endl;
}

/* Parses command-line options into a StrainConfiguration struct. */
StrainConfiguration ParseOptions(int argc, const char* argv[]) {

	/* Convert argv[] to a list of strings to make processing
	 * easier.
	 */
	vector<string> args(argv, argv + argc);

	/* Check that we have at least two args (so we have an
	 * input file)
	 */
	if (argc < 2) {
		PrintUsage();
		exit(0);
	}

	string inputFile = argv[1];
	bool isVerbose = false;

	/* Parse options */
	for (size_t i = 2; i < args.size(); ++i) {
		if (args[i] == "-v") {
			isVerbose = true;
		}
		else {
			cout << "Unknown option: " << args[i] << endl;
			exit(-1);
		}
	}

	/* Convert to a result struct. */
	StrainConfiguration result;
	result.isVerbose = isVerbose;
	result.generator = new CCodeGenerator();

	ifstream* input = new ifstream(inputFile.c_str());
	if (!input)
		throw runtime_error("Could not open input file: " + inputFile);
	result.inputFile = input;

	return result;
}

/* Helper function which parses input from a file into a string until it hits a line
 * of the form %%.
 */
void ReadDelimitedSection(istream& input, string& result) {
	result.clear();

	while (true) {
		/* Read a line from the file; if we fail, bail out with an error. */
		string line;
		if (!getline(input, line))
			throw runtime_error("Unexpected end of file.");

		/* If this line is identically %%, we're done. */
		if (line == "%%")
			break;

		/* Otherwise, append the line to the result. */
		result += line;
		result += '\n';
	}
}

/* Given an input file, reads the prototype section from the file. */
void ReadPrototypes(istream& input, StrainFile& result) {
	ReadDelimitedSection(input, result.prototypes);
}

/* Reads the implementation section from a file. */
void ReadImplementations(istream& input, StrainFile& result) {
	ReadDelimitedSection(input, result.implementations);
}

/* Reads all of the token types from the file. */
void ReadTokenTypes(istream& input, StrainFile& result) {
	/* Keep reading a line at a time until we have read all of the
	 * tokens and hit a '%%'.
	 */
	while (true) {
		/* Read a line and report an error if something goes wrong while doing so. */
		string line;
		if (!getline(input, line))
			throw runtime_error("Unexpected end of file reading token types.");

		/* If the line is '%%', we're done. */
		if (line == "%%")
			break;

		/* Split the token in two by writing locating the colon. */
		const size_t splitPoint = line.find(':');
		if (splitPoint == string::npos)
			throw runtime_error("Token must be annotated with a type.");

		const string token = line.substr(0, splitPoint);
		const string type = line.substr(splitPoint + 1);

		/* Confirm that:
		 * 1. We haven't seen this token before.
		 * 2. The token isn't empty.
		 * 3. The token consists solely of alphanumeric chars and the _ character.
		 */
		if (result.tokens.find(token) != result.tokens.end())
			throw runtime_error("Duplicate token: " + line);
		if (token.empty())
			throw runtime_error("Token names must be nonempty.");
		for (string::const_iterator itr = token.begin(); itr != token.end(); ++itr)
			if (!isalnum(*itr) && *itr != '_')
				throw runtime_error("Invalid character in token " + line);

		/* Add it to the list. */
		result.tokens[token] = type;
	}
}

/* Reads all of the regexps from the file. */
void ReadRegexps(istream& input, StrainFile& result) {
	/* The format for regexps is a line containing the regexp, followed by
	 * some number of lines of semantic action, and finally a blank line.
	 */
	while (true) {
		/* Read in the regexp.  If we fail, that's okay, since the file can end here. */
		string regexp;
		if (!getline(input, regexp))
			break;

		/* Now keep reading lines until we get a blank line. */
		string action;

		while (true) {
			/* These lines must be here. */
			string line;
			if (!getline(input, line))
				throw runtime_error("Unexpected EOF parsing action for regexp " + regexp);

			/* Stop at blank lines. */
			if (line.empty())
				break;

			/* The line has to start with whitespace. */
			if (!isspace(line[0]))
				throw runtime_error("Semantic action lines must start with whitespace.");

			/* Add everything else in. */
			action += line;
			action += '\n';
		}

		/* Add to the list. */
		result.regexps.push_back(make_pair(regexp, action));
	}
}


/* Parses the input file into a list of regexps and token names. */
StrainFile ReadFileContents(istream& input) {
	StrainFile result;

	ReadPrototypes(input, result);
	ReadImplementations(input, result);
	ReadTokenTypes(input, result);
	ReadRegexps(input, result);

	return result;
}

/**
 * Syntax:
 * strain inputfile [-o outputFile] [-v]
 */
int main(int argc, const char* argv[]) {
	try {
		/* Get configuration information. */
		StrainConfiguration config = ParseOptions(argc, argv);

		/* If verbose is not turned on, kill clog.  All operations which then
		 * try to write to it will not produce any output.
		 */
		if (!config.isVerbose)
			clog.setstate(ios::failbit);

		/* Get back the list of regexps and their names. */
		StrainFile fileContents = ReadFileContents(*config.inputFile);

		/* Log them. */
		for (size_t i = 0; i < fileContents.regexps.size(); ++i) {
			clog << "Regexp:  " << fileContents.regexps[i].first << endl;
			clog << "Action:  " << fileContents.regexps[i].second << endl;
		}

		/* Lex and parse each line. */
		vector<expr_t> regexps;
		for (size_t i = 0; i < fileContents.regexps.size(); ++i) {
			clog << "Lexing and parsing regexp: "
				<< fileContents.regexps[i].first << endl;
			stringstream inputStream(fileContents.regexps[i].first);
			Lexer lexer(inputStream);
			regexps.push_back(Parse(lexer));
		}

		/* Convert each to an automaton. */
		vector<RegexpAutomaton> automata;
		for (size_t i = 0; i < regexps.size(); ++i) {
			clog << "Converting regexp " << i << " to an automaton." << endl;

			/* Convert the regexp to an automaton.  The action number is its position
			 * in the list, and its priority is the same.  We may change this in the
			 * future if we allow multiple regexps to have the same action.
			 */
			automata.push_back(regexps[i]->toAutomaton(i, i));
		}

		/* Create a new e-NFA whose start state has epsilon transitions to
		 * each of these automata's start states.  This automaton is what
		 * ends up doing the lexing.
		 */
		RegexpAutomaton lexerENFA;
		lexerENFA.start = new MachineState;
		for (size_t i = 0; i < automata.size(); ++i)
			lexerENFA.start->eTransitions.insert(automata[i].start);

		/* Use the subset construction to build a single DFA for the entire
		 * world.
		 */
		clog << "Constructing lexer DFA." << endl;
		RegexpAutomaton lexerDFA = DoSubsetConstruction(lexerENFA);

		/* Minimize the DFA to reduce the number of states we produce. */
		clog << "Minimizing lexer DFA." << endl;
		MinimizeDFA(lexerDFA);

		/* Generate the lexer! */
		config.generator->writeLexer(lexerDFA, fileContents);

	}
	catch (const exception& e) {
		cout << e.what() << endl;
		return -1;
	}
}
