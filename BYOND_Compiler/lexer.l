// C/C++ tokenizer (plain, no Unicode)

%top{
#include "util.hpp"
enum class Kind {E
	
	DEFINE

};
%}


%class{
	std::stack<string_t> _include_stack;
	void include_file()
  {
    if (_include_stack.size() > 99)
      exit(EXIT_FAILURE);           // max include depth exceeded
	  std::string_view file(text());
	  file.remove_prefix(file.find_first_of('"'));
	  file.remove_suffix(file.size()-file.find_last_of('"'));// get filename    
	  _include_stack.push(file);
    FILE *fd = fopen(_include_stack.top().data(), "r");
    if (!fd)
      exit(EXIT_FAILURE);           // cannot open file
    push_matcher(new_matcher(fd));  // push current matcher, use new matcher
  }
  
  bool end_of_file()
  {
    if (depth == 0)
      return true;                  // return true: no more input to read
    fclose(in());                   // close current input in() (a FILE*)
    pop_matcher();                  // delete current matcher, pop matcher
	_include_stack.pop();
    return false;                   // return false: continue reading
  }

%}

%init{

%}


%option flex
%option nodefault
%option fast
%option main

/* may add %option unicode before the %include to match unicode identifiers */

// %include "cdefs.l" // That is the definisions below

// Requires RE/flex matcher or a Perl/ECMA matcher that supports lazy quants

WHITESPACE	[[:space:]]+
ILCOMMENT	"//"([^\n]|\\\n)*\n
MLCOMMENT	"/*"(.|\n)*?"*/"

NAME		[[:alpha:]_][[:alnum:]_]*
	^\h*#\h*
TILL_EOL	([^\n]|\\\r?\n)+
UFLT		\d*(\d|\.\d|\d\.)\d*([Ee][-+]?\d+)?[FfLl]?
UOCT		0[0-7]+
UHEX		0[Xx][[:xdigit:]]+
UDEC		0|[1-9]\d*
UINT		({UOCT}|{UHEX}|{UDEC})([Ll]{0,2}[Uu]?|[Uu][Ll]{0,2})
CHAR		L?'(\\.|[^\\\n'])*'
STRING		L?\"(\\.|\\\r?\n|[^\\\n"])*\"




%%

{WHITESPACE}
{ILCOMMENT}
{MLCOMMENT}
// handles include
{DIRECTIVE}include\h*\".*?\"    include_file();
<<EOF>>                   if (end_of_file()) return 0;

{DIRECTIVE}define{WHITESPACE}  start(DIRECTIVE_DEFINE);
<DIRECTIVE_DEFINE> NAME			


printf("DIRECTIVE %s\n", yytext);
{NAME}		printf("NAME      %s\n", yytext);
{UFLT}		printf("FLOAT     %s\n", yytext);
{UINT}		printf("INT       %s\n", yytext);
{CHAR}		printf("CHAR      %s\n", yytext);
{STRING}	printf("STRING    %s\n", yytext);
"{"|"<%"	|
"}"|"%>"	|
"["|"<:"	|
"]"|":>"	|
"("		|
")"		|
"+="		|
"++"		|
"+"		|
"-="		|
"--"		|
"->*"		|
"->"		|
"-"		|
"=="		|
"="		|
"<="		|
"<<="		|
"<<"		|
"<"		|
">="		|
">>="		|
">>"		|
">"		|
"!="		|
"!"		|
","		|
";"		|
"..."		|
".*"		|
"."		|
"^="		|
"^"		|
"~"		|
"*="		|
"*"		|
"/="		|
"/"		|
"%="		|
"%"		|
"&="		|
"&&"		|
"&"		|
"|="		|
"||"		|
"|"		|
"::"		|
":"		|
"?"		printf("PUNCT     %s\n", yytext);
.		printf("*** ERROR '%s' at line %d\n", yytext, yylineno);

%%

