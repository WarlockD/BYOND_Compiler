// C/C++ tokenizer (plain, no Unicode)

%top{
#include "util.hpp"
#include "Lex.hpp"

%}


%class{

	std::stack<string_t> _include_stack;

  Location get_location() {
	return Location(lineno(), columno());
  }

%}

%init{

%}
%option token-type=Token
//%option flex
//%option nodefault
%option fast


/* may add %option unicode before the %include to match unicode identifiers */

// %include "cdefs.l" // That is the definisions below

// Requires RE/flex matcher or a Perl/ECMA matcher that supports lazy quants

WHITESPACE	[[:space:]]+
ILCOMMENT	"//"([^\n]|\\\n)*\n
MLCOMMENT	"/*"(.|\n)*?"*/"

NAME		[[:alpha:]_][[:alnum:]_]*
TILL_EOL	([^\n]|\\\r?\n)+
UFLT		\d*(\d|\.\d|\d\.)\d*([Ee][-+]?\d+)?[FfLl]?
UOCT		0[0-7]+
UHEX		0[Xx][[:xdigit:]]+
UDEC		0|[1-9]\d*



DIRECTIVE	^[[:blank:]]*#([^\n]|\\\r?\n)+

UINT		({UOCT}|{UHEX}|{UDEC})([Ll]{0,2}[Uu]?|[Uu][Ll]{0,2})
CHAR		L?'(\\.|[^\\\n'])*'
STRING		L?\"(\\.|\\\r?\n|[^\\\n"])*\"

%%

//{WHITESPACE}
{ILCOMMENT}
{MLCOMMENT}
// handles include
{DIRECTIVE}include\h*\".*?\"   {
    if (_include_stack.size() > 99)
      exit(EXIT_FAILURE);           // max include depth exceeded
	  std::string_view file(text());
	  file.remove_prefix(file.find_first_of('"'));
	  file.remove_suffix(file.size()-file.find_last_of('"'));// get filename    
	  _include_stack.push(file);
    FILE *fd = fopen(_include_stack.top().c_str(), "r");
    if (!fd)
      exit(EXIT_FAILURE);           // cannot open file
    push_matcher(new_matcher(fd));  // push current matcher, use new matcher

    }



<<EOF>>  {
    if (_include_stack.size() == 0)
    {
       terminate();
    } else {
         fclose(in());                   // close current input in() (a FILE*)
    	_include_stack.pop();
        pop_matcher();                  // delete current matcher, pop matcher
        }
    }

// {DIRECTIVE}define{WHITESPACE}  start(DIRECTIVE_DEFINE);
// <DIRECTIVE_DEFINE> NAME			

{NAME}		return Token(text(), Punc::Name, get_location());
{UFLT}		return Token(text(), Punc::Float, get_location()); //static_cast<float>(0),get_location());
{UINT}		return Token(text(), Punc::Int, get_location()); ///static_cast<int>(0),get_location());
{CHAR}		return Token(text(), Punc::Char, get_location());
{STRING}	return Token(text(), Punc::String, get_location());
"\t"	return Token(Punc::Tab,get_location());
"\n"	return Token(Punc::Newline,get_location());
" "		return Token(Punc::Space,get_location());
"!"		return Token(Punc::Not,get_location());
"!="	return Token(Punc::NotEq,get_location());
"\""	return Token(Punc::DoubleQuote,get_location());
"#"		return Token(Punc::Hash,get_location());
"##"	return Token(Punc::TokenPaste,get_location());
"%"		return Token(Punc::Mod,get_location());
"%="	return Token(Punc::ModAssign,get_location());
"&"		return Token(Punc::BitAnd,get_location());
"&&"	return Token(Punc::And,get_location());
"&="	return Token(Punc::BitAndAssign,get_location());
"'"		return Token(Punc::SingleQuote,get_location());
"("		return Token(Punc::LParen,get_location());
")"		return Token(Punc::RParen,get_location());
"*"		return Token(Punc::Mul,get_location());
"**"	return Token(Punc::Pow,get_location());
"*="	return Token(Punc::MulAssign,get_location());
"+"		return Token(Punc::Add,get_location());
"++"	return Token(Punc::PlusPlus,get_location());
"+="	return Token(Punc::AddAssign,get_location());
","		return Token(Punc::Comma,get_location());
"-"		return Token(Punc::Sub,get_location());
"--"	return Token(Punc::MinusMinus,get_location());
"-="	return Token(Punc::SubAssign,get_location());
"."		return Token(Punc::Dot,get_location());
".."	return Token(Punc::Super,get_location());
"..."	return Token(Punc::Ellipsis,get_location());
"/"		return Token(Punc::Slash,get_location());
"/*"	return Token(Punc::BlockComment,get_location());
"//"	return Token(Punc::LineComment,get_location());
"/="	return Token(Punc::DivAssign,get_location());
":"		return Token(Punc::Colon,get_location());//-> CloseColon;
";"		return Token(Punc::Semicolon,get_location());
"<"		return Token(Punc::Less,get_location());
"<<"	return Token(Punc::LShift,get_location());
"<<="	return Token(Punc::LShiftAssign,get_location());
"<="	return Token(Punc::LessEq,get_location());
"<>"	return Token(Punc::LessGreater,get_location());
"="		return Token(Punc::Assign,get_location());
"=="	return Token(Punc::Eq,get_location());
">"		return Token(Punc::Greater,get_location());
">="	return Token(Punc::GreaterEq,get_location());
">>"	return Token(Punc::RShift,get_location());
">>="	return Token(Punc::RShiftAssign,get_location());
"?"		return Token(Punc::QuestionMark,get_location());
"?."	return Token(Punc::SafeDot,get_location());
"?:"	return Token(Punc::SafeColon,get_location());
"["		return Token(Punc::LBracket,get_location());
"]"		return Token(Punc::RBracket,get_location());
"^"		return Token(Punc::BitXor,get_location());
"^="	return Token(Punc::BitXorAssign,get_location());
"{"		return Token(Punc::LBrace,get_location());
"{\""	return Token(Punc:: BlockString,get_location());
"|"		return Token(Punc::BitOr,get_location());
"|="	return Token(Punc::BitOrAssign,get_location());
"||"	return Token(Punc::Or,get_location());
"}"		return Token(Punc::RBrace,get_location());
"~"		return Token(Punc::BitNot,get_location());
"~!"	return Token(Punc::NotEquiv,get_location());
"~="	return Token(Punc::Equiv,get_location());

.		
        return Token(Punc::Null,get_location());  //printf("*** ERROR '%s' at line %d\n", yytext, yylineno);

%%

