// C/C++ tokenizer (plain, no Unicode)

%top{
#include "util.hpp"


enum class Tok {
	Null, // empty
	Unkonwn,
	Tab,
	Newline,
	Space,
	Not,
	NotEq,
	DoubleQuote,
	Hash,
	TokenPaste,
	Mod,
	ModAssign,
	BitAnd,
	And,
	BitAndAssign,
	SingleQuote,
	LParen,
	RParen,
	Mul,
	Pow,
	MulAssign,
	Add,
	PlusPlus,
	AddAssign,
	Comma,
	Sub,
	MinusMinus,
	SubAssign,
	Dot,
	Super,
	Ellipsis,
	Slash,
	BlockComment,
	LineComment,
	DivAssign,
	Colon,

	Semicolon,
	Less,
	LShift,
	LShiftAssign,
	LessEq,
	LessGreater,
	Assign,
	Eq,
	Greater,
	GreaterEq,
	RShift,
	RShiftAssign,
	QuestionMark,
	SafeDot,
	SafeColon,
	LBracket,
	RBracket,
	BitXor,
	BitXorAssign,
	LBrace,
	BlockString,
	BitOr,
	BitOrAssign,
	Or,
	RBrace,
	BitNot,
	NotEquiv,
	Equiv,
	// Keywords - not checked by read_punct
	In,
	// Tokens
	Sharp,
	Fixed,
	Float,
	Char,
	String,
	Name,
	CommentLine,
	CommentBlock
};
template<typename ... ARGS>
static inline int Token(Tok v, ARGS ... args) { return static_cast<int>(v);}
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
    FILE *fd = fopen(_include_stack.top().c_str(), "r");
    if (!fd)
      exit(EXIT_FAILURE);           // cannot open file
    push_matcher(new_matcher(fd));  // push current matcher, use new matcher
  }
  
  bool end_of_file()
  {
    if (_include_stack.size() == 0)
      return true;                  // return true: no more input to read
    fclose(in());                   // close current input in() (a FILE*)
    pop_matcher();                  // delete current matcher, pop matcher
	_include_stack.pop();
    return false;                   // return false: continue reading
  }

  Location get_location() {
	return Location(lineno(), columno());
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

{WHITESPACE}
{ILCOMMENT}
{MLCOMMENT}
// handles include
{DIRECTIVE}include\h*\".*?\"    include_file();
<<EOF>>                   if (end_of_file()) return 0;

// {DIRECTIVE}define{WHITESPACE}  start(DIRECTIVE_DEFINE);
// <DIRECTIVE_DEFINE> NAME			

{NAME}		return Token(Tok::Tab,get_location());
{UFLT}		return Token(Tok::Float, get_location()); //static_cast<float>(0),get_location());
{UINT}		return Token(Tok::Fixed, get_location()); ///static_cast<int>(0),get_location());
{CHAR}		printf("CHAR      %s\n", yytext);
{STRING}	printf("STRING    %s\n", yytext);
"\t"	return Token(Tok::Tab,get_location());
"\n"	return Token(Tok::Newline,get_location());
" "		return Token(Tok::Space,get_location());
"!"		return Token(Tok::Not,get_location());
"!="	return Token(Tok::NotEq,get_location());
"\""	return Token(Tok::DoubleQuote,get_location());
"#"		return Token(Tok::Hash,get_location());
"##"	return Token(Tok::TokenPaste,get_location());
"%"		return Token(Tok::Mod,get_location());
"%="	return Token(Tok::ModAssign,get_location());
"&"		return Token(Tok::BitAnd,get_location());
"&&"	return Token(Tok::And,get_location());
"&="	return Token(Tok::BitAndAssign,get_location());
"'"		return Token(Tok::SingleQuote,get_location());
"("		return Token(Tok::LParen,get_location());
")"		return Token(Tok::RParen,get_location());
"*"		return Token(Tok::Mul,get_location());
"**"	return Token(Tok::Pow,get_location());
"*="	return Token(Tok::MulAssign,get_location());
"+"		return Token(Tok::Add,get_location());
"++"	return Token(Tok::PlusPlus,get_location());
"+="	return Token(Tok::AddAssign,get_location());
","		return Token(Tok::Comma,get_location());
"-"		return Token(Tok::Sub,get_location());
"--"	return Token(Tok::MinusMinus,get_location());
"-="	return Token(Tok::SubAssign,get_location());
"."		return Token(Tok::Dot,get_location());
".."	return Token(Tok::Super,get_location());
"..."	return Token(Tok::Ellipsis,get_location());
"/"		return Token(Tok::Slash,get_location());
"/*"	return Token(Tok::BlockComment,get_location());
"//"	return Token(Tok::LineComment,get_location());
"/="	return Token(Tok::DivAssign,get_location());
":"		return Token(Tok::Colon,get_location());//-> CloseColon;
";"		return Token(Tok::Semicolon,get_location());
"<"		return Token(Tok::Less,get_location());
"<<"	return Token(Tok::LShift,get_location());
"<<="	return Token(Tok::LShiftAssign,get_location());
"<="	return Token(Tok::LessEq,get_location());
"<>"	return Token(Tok::LessGreater,get_location());
"="		return Token(Tok::Assign,get_location());
"=="	return Token(Tok::Eq,get_location());
">"		return Token(Tok::Greater,get_location());
">="	return Token(Tok::GreaterEq,get_location());
">>"	return Token(Tok::RShift,get_location());
">>="	return Token(Tok::RShiftAssign,get_location());
"?"		return Token(Tok::QuestionMark,get_location());
"?."	return Token(Tok::SafeDot,get_location());
"?:"	return Token(Tok::SafeColon,get_location());
"["		return Token(Tok::LBracket,get_location());
"]"		return Token(Tok::RBracket,get_location());
"^"		return Token(Tok::BitXor,get_location());
"^="	return Token(Tok::BitXorAssign,get_location());
"{"		return Token(Tok::LBrace,get_location());
"{\""	return Token(Tok:: BlockString,get_location());
"|"		return Token(Tok::BitOr,get_location());
"|="	return Token(Tok::BitOrAssign,get_location());
"||"	return Token(Tok::Or,get_location());
"}"		return Token(Tok::RBrace,get_location());
"~"		return Token(Tok::BitNot,get_location());
"~!"	return Token(Tok::NotEquiv,get_location());
"~="	return Token(Tok::Equiv,get_location());
// Keywords - not checked by read_punct
"in"	return Token(Tok::In,get_location());
	
	
.		printf("*** ERROR '%s' at line %d\n", yytext, yylineno);

%%

