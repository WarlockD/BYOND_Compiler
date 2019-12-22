/* Scanner for "C" assignment statements... sort of. */
/* Compile: reflex --bison-bridge --header-file reflexexample4.l */
/* compile: bison -d -y flexexample4.y */
/* Example taken from Flex documentation A.2 */
/* Bison bridge passes yylval to yylex (which limits yylval's scope to the rule actions) */
/* Note: yylval is a reference, no longer a pointer with reflex */

%top{
#include "y.tab.h"  /* Generated by bison. */
%}

%option bison-bridge

%%

[[:digit:]]+  { yylval.num = atoi(text());   return NUMBER; }
[[:alnum:]]+  { yylval.str = strdup(text()); return STRING; }
"="|";"       { return text()[0]; }
.|\n          {}

%%