%option     outfile="lexer.cxx"


%option c++
%option 8bit warn nodefault
%option noyywrap

%{
    #include <stdexcept>
    #include <cstdlib>
    #include "parser.hxx"
    #include "scanner.hpp"

    // utility macros to simplify the actions
    #define YIELD_TOKEN(tok, val, type)                 \
                    yylval->build<type>(val);           \
                    return yy::parser::token::T_##tok;

    #define YY_TXT                  std::string(yytext, yyleng)
    #define YY_NUM                  std::atof(yytext)

    #define INT_TOKEN(tok, val)     YIELD_TOKEN(tok, val, int)
    #define NUM_TOKEN(tok)          YIELD_TOKEN(tok, YY_NUM, double)
    #define STR_TOKEN(tok)          YIELD_TOKEN(tok, YY_TXT, std::string)
    #define LITERAL                 return yytext[0];

    // before executing an action, set the length of the location from
    // the length of the matched pattern:
    #define YY_USER_ACTION          yylloc->columns(yyleng);
%}

%%
%{
    // before matching any pattern, update the the current location
    yylloc->step();
%}

I/\(                        LITERAL
H/\(                        LITERAL

[[:alpha:]][[:alnum:]_]*    STR_TOKEN(NAME)

[[:digit:]]+                NUM_TOKEN(NUM)
[[:digit:]]*\.[[:digit:]]+  NUM_TOKEN(NUM)

\+                          INT_TOKEN(SIGN, ast::SIGN_PLUS)
\-                          INT_TOKEN(SIGN, ast::SIGN_MINUS)

==?                         INT_TOKEN(REL, ast::REL_EQ)
\<=                         INT_TOKEN(REL, ast::REL_LE)
\>=                         INT_TOKEN(REL, ast::REL_GE)

#.*                         {/* eat comments */}
[ \t]                       {/* eat whitespace */}

\n                          yylloc->lines(1); LITERAL

                            /* forward everything else, even invalid
                             * tokens - making use of bison's automatic
                             * error messages */
.                           LITERAL

%%

yy::scanner::scanner(std::istream* in, std::ostream* out)
    : yyFlexLexer(in, out)
{
}

// Flex generates the code for `yy::scanner::lex` (see YY_DECL).

// This must be defined manually to prevent linker errors:
int yyFlexLexer::yylex()
{
    throw std::logic_error(
        "The yylex() exists for technical reasons and must not be used.");
}