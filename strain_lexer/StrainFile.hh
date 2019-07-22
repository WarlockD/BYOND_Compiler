/***************************************************
 * File: StrainFile.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * An object representing the data in a strain input
 * file.
 */

#ifndef StrainFile_Included
#define StrainFile_Included

#include <string>
#include <vector>
#include <map>

struct StrainFile {
	/* A string containing the prototypes that will be included in the .cpp file. */
	std::string prototypes;

	/* A string containing the implementations that will be included in the .cpp file. */
	std::string implementations;

	/* A map from token types to the C++ type that they are backed by. */
	std::map<std::string, std::string> tokens;

	/* A list of (regexp, action) pairs. */
	std::vector<std::pair<std::string, std::string> > regexps;
};

#endif