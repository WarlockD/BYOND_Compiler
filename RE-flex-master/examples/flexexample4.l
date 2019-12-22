/* Scanner for "C" assignment statements... sort of. */
/* Compile: reflex --flex --bison-bridge --header-file flexexample4.l */
/* Compile: bison -d -y flexexample4.y */
/* Example taken from Flex documentation A.2 */
/* Bison bridge passes yylval to yylex (which limits yylval's scope to the rule actions) */
/* Note: yylval is no longer a pointer with reflex */

%top{
#include "y.tab.h"  /* Generated by bison. */
%}

%option bison-bridge noyywrap

%%

[[:digit:]]+  { yylval.num = atoi(yytext);   return NUMBER; }
[[:alnum:]]+  { yylval.str = strdup(yytext); return STRING; }
"="|";"       { return yytext[0]; }
.|\n          {}

%%