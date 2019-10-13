#pragma once
#include "util.hpp"
#include <tuple>
#include <variant>



static inline constexpr int toKind(char c) { return static_cast<uint8_t>(c); }
static inline constexpr int toKind(const char c[2]) { return (static_cast<uint8_t>(c[1]) << 8) || static_cast<uint8_t>(c[0]); }
static inline constexpr int toKindIdent(size_t i,char c = ' ') { return static_cast<uint16_t>(c)<< 16 | static_cast<uint8_t>(c); }
static inline constexpr uint16_t getKind(int i) { return static_cast<uint16_t>(i); }
static inline constexpr uint16_t getIdent(int i) { return static_cast<uint16_t>(i >> 16); }

class FilePosition {
	string_t _filename;
	size_t _line;
	size_t _pos;
public:
	FilePosition() : _filename(), _line(0), _pos(0) {}
	FilePosition(string_t filename, size_t line, size_t pos) : _filename(filename), _line(line), _pos(pos) {}
	size_t pos() const { return _pos; }
	size_t lineno() const { return _line; }
	const string_t& filename() const { return _filename; }
	bool operator==(const FilePosition& r) const { return _filename == r._filename && _line == r._line && _pos == r._pos; }
	friend class Lexer;
};


class Lexer {


	enum class Tok {
		Null, // empty
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
	};

	enum  kwtype {
		KIF, KIFDEF, KIFNDEF, KELIF, KELSE, KENDIF, KINCLUDE, KDEFINE,
		KUNDEF, KLINE, KERROR, KPRAGMA, KDEFINED,
		KLINENO, KFILE, KDATE, KTIME, KSTDC, KEVAL
	};

	/* character classes */
	struct C_WS {		constexpr bool operator()(int ch) const { return ch == ' ' || ch == '\t' || ch == '\n'; } };	//1
	struct C_ALPH {		constexpr bool operator()(int ch) const { return isalpha(ch); } };	//1
	struct C_NUM {		constexpr bool operator()(int ch) const { return isdigit(ch); } };	//1
	struct C_EOF { constexpr bool operator()(int ch) const { return ch == EOF; }; };	//1
	struct C_XX { constexpr bool operator()(int ch) const { return true; } };// any
	struct C_CHAR { const int _ch = 0;	constexpr bool operator()(int ch) const { return _ch == ch; } };// any

	int	tottok;
	int	tokkind[256];
	static constexpr size_t MAXSTATE = 32;
	enum class STATE {
		START = 0, NUM1, NUM2, NUM3, ID1, ST1, ST2, ST3, COM1, COM2, COM3, COM4,
		CC1, CC2, WS1, PLUS1, MINUS1, STAR1, SLASH1, PCT1, SHARP1,
		CIRC1, GT1, GT2, LT1, LT2, OR1, AND1, ASG1, NOT1, DOTS1,
		S_SELF = MAXSTATE, S_SELFB, S_EOF, S_NL, S_EOFSTR,
		S_STNL, S_COMNL, S_EOFCOM, S_COMMENT, S_EOB, S_WS, S_NAME
	};

	
	static constexpr size_t	QBSBIT = 0100;

	using cclass = std::variant<std::nullptr_t, C_WS, C_ALPH, C_NUM, C_EOF, C_XX, C_CHAR>;
	struct action_t {
		const Tok token; const STATE state;
	};
	static constexpr action_t ACT(Tok tok, STATE act) { return action_t{ tok, act }; }
	static constexpr STATE	GETACT(action_t st) { return st.state; }

	using nstate_t = std::variant<STATE, action_t>;

	struct	fsm_t {
		const STATE	state;		/* if in this state */
		const cclass ch[4];		/* and see one of these characters */
		const STATE	nextstate;	/* enter this state if +ve */
	};

	static constexpr  fsm_t fsm[] {
		/* start state */
		{ STATE::START,	{ C_XX{} },	STATE::S_SELF },

	};

};
#if 0
 class Punc {
	 // from spacemanDMM
	 /// This lookup table is used to keep `read_punct`, called for essentially each
/// character in the input, blazing fast. The code to generate it is contained
/// in the following test.
	 static constexpr std::pair<size_t, size_t> SPEEDY_TABLE[127] = {
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 1 }, { 1, 2 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 2, 3 }, { 3, 5 }, { 5, 6 }, { 6, 8 }, { 0, 0 }, { 8, 10 }, { 10, 13 }, { 13, 14 },
		 { 14, 15 }, { 15, 16 }, { 16, 19 }, { 19, 22 }, { 22, 23 }, { 23, 26 }, { 26, 29 }, { 29, 33 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 33, 34 }, { 34, 35 }, { 35, 40 }, { 40, 42 }, { 42, 46 }, { 46, 49 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 49, 50 }, { 0, 0 }, { 50, 51 }, { 51, 53 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
		 { 0, 0 }, { 0, 0 }, { 0, 0 }, { 53, 55 }, { 55, 58 }, { 58, 59 }, { 59, 62 }
	 };
 public:
	 enum _PEnum {
		 Null, // empty
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
	 };
	 static constexpr auto  CloseColon = Colon;
	 inline operator _PEnum() const { return _value; }
	 static constexpr const char* _toString[] = {
		 "\0",
	 "\t",
	 "\n",
	 " ",
	 "!",
	 "!=",
	 "\"",
	 "#",
	 "##",
	 "%",
	 "%=",
	 "&",
	 "&&",
	 "&=",
	 "'",
	 "(",
	 ")",
	 "*",
	 "**",
	 "*=",
	 "+",
	 "++",
	 "+=",
	 "\"",
	 "-",
	 "--",
	 "-=",
	 ".",
	 "..",
	 "...",
	 "/",
	 "/*",
	 "//",
	 "/=",
	 ":",
	 ";",
	 "<",
	 "<<",
	 "<<=",
	 "<=",
	 "<>",
	 "=",
	 "==",
	 ">",
	 ">=",
	 ">>",
	 ">>=",
	 "?",
	 "?.",
	 "?:",
	 "[",
	 "]",
	 "^",
	 "^=",
	 "{",
	 "{\"",
	 "|",
	 "|=",
	 "||",
	 "}",
	 "~",
	 "~!",
	 "~=",
	 // Keywords - not checked by read_punct
	 "in",
	 };
	 constexpr Punc() : _value(Null) {}
	 constexpr Punc(_PEnum v) : _value(v) {}
	 constexpr bool is_whitespace() const { return _value == Tab || _value == Newline || _value == Space; }
	 constexpr bool operator==(const _PEnum& v) const { return _value == v; }
	 constexpr bool operator!=(const _PEnum& v) const { return _value != v; }
 private:
	 _PEnum _value;
 };
 // enum class Interp { None, Begin, Part, End };

 struct Symbol { string_t value; };
 struct IdentSymbol : public Symbol {  }; // true
 struct String { string_t value; };
 struct InterpStringBegin { string_t value; };
 struct InterpStringPart { string_t value; };
 struct InterpStringEnd { string_t value; };
 struct DocComment { string_t comment; };
 static bool operator==(const Symbol& l, const string_t& r) { return l.value == r; }
 static bool operator!=(const Symbol& l, const string_t& r) { return l.value != r; }
 static bool operator==(const IdentSymbol& l, const string_t& r) { return l.value == r; }
 static bool operator!=(const IdentSymbol& l, const string_t& r) { return l.value != r; }

 static bool operator==(const InterpStringBegin& l, const string_t& r) { return l.value == r; }
 static bool operator!=(const InterpStringBegin& l, const string_t& r) { return l.value != r; }

 static bool operator==(const InterpStringPart& l, const string_t& r) { return l.value == r; }
 static bool operator!=(const InterpStringPart& l, const string_t& r) { return l.value != r; }

 static bool operator==(const InterpStringEnd& l, const string_t& r) { return l.value == r; }
 static bool operator!=(const InterpStringEnd& l, const string_t& r) { return l.value != r; }

 using Resource = std::filesystem::path;



 using token_t = std::variant<std::nullptr_t, Punc, Symbol, IdentSymbol, String, InterpStringBegin, InterpStringPart, InterpStringEnd, Resource, DocComment,  int, float>;

 class Token {
	 token_t _value;
	 FilePosition _fpos;
 public:
	 Token() :_value(nullptr), _fpos() {}
	 Token(int value, FilePosition fpos) : _value(value), _fpos(fpos) {}
	 Token(float value, FilePosition fpos) : _value(value), _fpos(fpos) {}
	 Token(std::filesystem::path value, FilePosition fpos) : _value(value), _fpos(fpos) {}
	 Token(string_t value, FilePosition fpos, bool spacebefore = false) : _value(spacebefore ? IdentSymbol{ value } : Symbol{ value }), _fpos(fpos) {}
	// Token(string_t value, FilePosition fpos, Interp type = Interp::None) : _value(stringlit_t(value, type)), _fpos(fpos) {}
	 operator const token_t& () const { return _value; }
	 bool isPunc() const { return std::holds_alternative<Punc>(_value); }
	 Punc punc() const { return std::get<Punc>(_value); }
	 constexpr bool is_whitespace() const {
		 if (auto v = std::get_if<Punc>(&_value))
			 return *v == Punc::Space || *v == Punc::Newline || *v == Punc::Tab;
		 else
			 return false;
	 }
	 constexpr bool is_ident() const { return std::holds_alternative<IdentSymbol>(_value) || std::holds_alternative<Symbol>(_value); }
	 string_t get_symbol() const { return std::holds_alternative<Symbol>(_value) ? std::get<Symbol>(_value).value :  std::get<IdentSymbol>(_value).value; }
	 bool is_ident(string_t match) const { return is_ident() ? get_symbol() == match : false;  }
	 bool seperate_from(const Token& v) const {
		 if (auto p = std::get_if<Punc>(&_value))
			 if (*p == Punc::In ||
				 *p == Punc::Eq ||
				 *p == Punc::NotEq ||
				 *p == Punc::Mod ||
				 *p == Punc::And ||
				 *p == Punc::BitAndAssign ||
				 *p == Punc::Mul ||
				 *p == Punc::Pow ||
				 *p == Punc::MulAssign ||
				 *p == Punc::Add ||
				 *p == Punc::AddAssign ||
				 *p == Punc::Sub ||
				 *p == Punc::SubAssign ||
				 *p == Punc::DivAssign ||
				 *p == Punc::Colon ||
				 *p == Punc::Less ||
				 *p == Punc::LShift ||
				 *p == Punc::LShiftAssign ||
				 *p == Punc::LessEq ||
				 *p == Punc::LessGreater ||
				 *p == Punc::Assign ||
				 *p == Punc::Greater ||
				 *p == Punc::GreaterEq ||
				 *p == Punc::RShift ||
				 *p == Punc::RShiftAssign ||
				 *p == Punc::QuestionMark ||
				 *p == Punc::BitXorAssign ||
				 *p == Punc::BitOrAssign ||
				 *p == Punc::Or
				) return true;// return true on these punc

#if 0
		 std::visit(overload{
			 [](const IdentSymbol&, const auto) { return true;  },
			 [](const Punc& p, const auto) { return p == ',';  },
			 [](const IdentSymbol&,const  Punc&) { return false;  },
			 [](const IdentSymbol&,const  InterpStringEnd&) { return false;  },
			 [](const IdentSymbol&, const InterpStringPart&) { return false;  },
			 [](const Symbol&,const  Punc&) { return false;  },
			 [](const Symbol&, const InterpStringEnd&) { return false;  },
			 [](const Symbol&, const InterpStringPart&) { return false;  },
			 [](const Punc&, const IdentSymbol&) { return false;  },
			 [](const Punc&, const Symbol&) { return false;  },
			 [](const InterpStringBegin&, const IdentSymbol&) { return false;  },
			 [](const InterpStringBegin&, const Symbol&) { return false;  },
			 [](const InterpStringPart&, const IdentSymbol&) { return false;  },
			 [](const InterpStringPart&, const Symbol&) { return false;  },

			 [](IdentSymbol&, auto&) { return true;  },
			 [](auto&, IdentSymbol&) { return true;  },
			 [](Symbol&, auto&) { return true;  },
			 [](auto&, Symbol&) { return true;  },

			 }, _value, v);
#endif 
	 }
 };


 struct Interpolation {
	 uint8_t* end;
	 size_t bracket_depth;
 };




class File {
	std::unique_ptr<std::istream> _ss;
	string_t _filename; // or macro
	size_t _lineno;

	File(std::istream* p, string_t filename=string_t()) : _ss(std::unique_ptr<std::istream>(p)), _filename(filename), _lineno(0){}
	File(std::istringstream* p) : _ss(std::unique_ptr<std::istream>(p)), _filename(""), _lineno(0) {}
public:
	File() : _ss(nullptr), _filename(), _lineno(0){}
	static File open_file(string_t filename) {
		std::fstream* f = new std::fstream(static_cast<std::string_view>(filename), std::ios::in);
		if (!f->good()) {
			delete f;
			throw 0; // fix this
		}
		return std::move(File(f, filename));
	}
	static File open_string(string_t s) {
		return std::move(File(new std::istringstream(std::string(static_cast<std::string_view>(s)))));
	}
	bool is_file() const { return _filename != string_t(); }
	string_t filename() const { return _filename; }
	operator std::istream& () { return *_ss; }
	std::istream* operator->() { return _ss.get(); }
	std::optional<size_t> read_line(std::string& line) {
		while (_ss->eof() || _ss->bad()) {
			int ch;
			bool line_empty = true;
			_lineno++;
			while ((ch = _ss->get()) != -1 && (ch != '\r') && (ch != 'n')) {
				if (!isspace(ch)) line_empty = false;
				line.push_back(ch);
			}
			int peek = _ss->peek();
			if (ch != peek && (peek == '\r' || peek == '\n'))
				_ss->get();// skip windows line endings
			if (!line_empty) break; // skip blank lines
		}
		if (line.empty()) return std::nullopt;
		else return _lineno;
	}
};
using tokenrow_t = std::vector<Token>;

class Lexer {

	std::stack<File> _fileStack;

	struct Define {
		string_t name;
		std::vector<string_t> _args;
		std::vector< Token> line;
	};
	size_t _ifStack;
	constexpr static inline bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }
	constexpr static inline bool is_alpha(char ch) { return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_'; }
	bool read_row(tokenrow_t& tokens) {
#if 0
		std::string line;
		while(_fileStack.empty()) {
			File& file = _fileStack.top();
			auto lineno = file.read_line(line);
			if (lineno.has_value()) {
				FilePosition pos(file.filename(), lineno.value(), 0);



			}
		} while (line.empty());
#endif
		return false;
	}
public:
};


#if 0
namespace _private {

	template<typename T>
	int ctrans(T& ss)
	{
		int c = *ss;
		if (c == '\\') {
			int k = 0;
			switch (c = *++ss)
			{
			case 'n': return '\n';
			case 't': return'\t';
			case 'r': return '\r';
			case 'b':return '\b';
			case 'f': return '\f'; 		/* form feed for ascii */
			case '\\': return '\\';
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7': {
				int k;
				c -= '0';
				while ((k = (*ss + 1)) >= '0' && k <= '7')
				{
					c = c * 8 + k - '0';
					++ss;
				}
				break;
			}
			};
		}
		return c;
	}
	int usescape(int c) {
		int d;
		switch (c) {
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case 't': c = '\t'; break;
		case 'b': c = '\b'; break;
		case 'f': c = 014; break;		/* form feed for ascii */
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			c -= '0';
			while ('0' <= (d = gch()) && d <= '7') {
				c = c * 8 + (d - '0');
				if (!('0' <= peek && peek <= '7')) break;
			}
			break;
		}
		return(c);
	}
	template<typename K, typename L>
	std::optional<size_t> lookup(const K& key, const L& list) {
		for (size_t i = 0; i < list.size(); i++) {
			if (list.at(i) == key) return i;
		}
		return nullptr;
	}
};
#endif

#endif