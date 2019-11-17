#pragma once
#include "util.hpp"
#include <tuple>
#include <variant>
#include <iostream>
#include <ostream>
#include <bitset>


static inline constexpr int toKind(char c) { return static_cast<uint8_t>(c); }
static inline constexpr int toKind(const char c[2]) { return (static_cast<uint8_t>(c[1]) << 8) || static_cast<uint8_t>(c[0]); }
static inline constexpr int toKindIdent(size_t i,char c = ' ') { return static_cast<uint16_t>(c)<< 16 | static_cast<uint8_t>(c); }
static inline constexpr uint16_t getKind(int i) { return static_cast<uint16_t>(i); }
static inline constexpr uint16_t getIdent(int i) { return static_cast<uint16_t>(i >> 16); }



class DocComment {
	bool _enclosedTarget;
	bool _blockComment;
	string_t _text;
public:
	DocComment(bool enclosedTarget, bool blockComment, string_t text) : _enclosedTarget(enclosedTarget), _blockComment(blockComment), _text(text) {}
	string_t text() const { return _text; }
	bool enclosedTarget() const { return _enclosedTarget; }
	bool blockComment() const { return _blockComment; }
};

static inline std::ostream& operator>>(std::ostream& os, const DocComment& c) {
	if (c.blockComment())
		if (c.enclosedTarget())
			os << "/*!" << c.text() << "*/";
		else
			os << "/**" << c.text() << "*/";
	else
		if (c.enclosedTarget())
			os << "//!" << c.text();
		else
			os << "///" << c.text();
	return os;
}

// DocCollection is justy a vector


template<typename T>
constexpr static inline bool HasLocation(T&) { return std::is_member_function_pointer<decltype(&A::location)>::value; }
template<typename T>
constexpr static inline bool HasLocation(const T&) { return std::is_member_function_pointer<decltype(&A::location)>::value; }

enum class Severity {
	Error = 1,
	Warning = 2,
	Info = 3,
	Hint = 4,
};


class DMError {
	struct DiagnosticNote {
		Location location;
		std::string text;
	};
	Location _location;
	Severity _severity;
	std::string _description;
	std::vector<DiagnosticNote> _notes;
	// cause  Option<Box<dyn error::Error + Send + Sync>>,
public:
	DMError(Location l, std::string desc) : _location(l), _description(desc), _severity(Severity::Error), _notes() {}

};



class Context {
	static std::mutex context_mutex;
	std::vector<string_t> _files;
	std::unordered_map<string_t, FileId> _reverse_files;
	std::vector< DMError> _errors;
	Severity _print_severity;
public:
	constexpr static uint16_t FILEID_BUILTINS = 0;
	constexpr static uint16_t FILEID_MIN = 1;
	constexpr static uint16_t FILEID_MAX = 0xfffe;
	constexpr static uint16_t FILEID_BAD= 0xffff;
	Context() : _print_severity(Severity::Warning), _files{ string_t("") }{}
	string_t get_file(size_t i) const { return _files[i]; }
	std::filesystem::path file_path(FileId file)  {
		if(file == FILEID_BUILTINS) {
			return "(builtins)";
		}
		auto idx = file - FILEID_MIN;
		if (idx > _files.size()) return "(unknown)";
		return static_cast<std::string_view>(_files[idx]);
	}
	FileId register_file(string_t s) {
		if (auto it = _reverse_files.find(s); it != _reverse_files.end())
			return it->second;
		std::lock_guard<std::mutex> lock(context_mutex);
		FileId idx = static_cast<FileId::type>(_files.size() + FILEID_MIN) ;
		_files.emplace_back(s);
		_reverse_files.emplace(s, idx);
		return idx;
	}
	void  register_error(DMError&& error) { 
		std::lock_guard<std::mutex> lock(context_mutex);
		_errors.emplace_back(std::move(error)); 
	}

};



enum class Punc {
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
	Number,
	Char,
	String,
	Name,
	CommentLine,
	CommentBlock
};


struct Ident { string_t name; bool ws_following; };
struct InterpStringBegin { string_t string; };
struct InterpStringPart { string_t string; };
struct InterpStringEnd { string_t string; };
struct Resource { string_t filename; };


using TokenValue = std::variant<Punc, Ident, Ident, InterpStringBegin, InterpStringPart, InterpStringEnd, Resource, DocComment,float,int32_t>;

struct Token {
	TokenValue _token;
	Location _location;
public:
	Token(const TokenValue& v, const Location& loc= Location()) : _token(v), _location(loc) {}
	const TokenValue& token() const { return _token; }
	const Location& location() const { return _location; }
};


class Lexer {


	enum  kwtype {
		KIF, KIFDEF, KIFNDEF, KELIF, KELSE, KENDIF, KINCLUDE, KDEFINE,
		KUNDEF, KLINE, KERROR, KPRAGMA, KDEFINED,
		KLINENO, KFILE, KDATE, KTIME, KSTDC, KEVAL
	};

	/* character classes */
	struct C_WS { constexpr C_WS() {} constexpr bool operator()(int ch) const { return ch == ' ' || ch == '\t' || ch == '\n'; } };	//1
	struct C_ALPH { constexpr C_ALPH() {}	constexpr bool operator()(int ch) const { return ('A' >= ch && 'Z' <= ch) || ('a' >= ch && 'z' <= ch); } };	//1
	struct C_NUM { constexpr C_NUM() {}constexpr bool operator()(int ch) const { return  ('0' >= ch && '9' <= ch); } };	//1
	struct C_EOF { constexpr C_EOF() {} constexpr bool operator()(int ch) const { return ch == EOF; }; };	//1
	struct C_XX { constexpr C_XX() {}constexpr bool operator()(int ch) const { return true; } };// any
	struct C_CHAR { constexpr C_CHAR() {}const int _ch = 0;	constexpr bool operator()(int ch) const { return _ch == ch; } };// any

	int	tottok;
	int	tokkind[256];
	//static constexpr size_t MAXSTATE = 32;
	 enum   STATE {
		START = 0, NUM1, NUM2, NUM3, ID1, ST1, ST2, ST3, COM1, COM2, COM3, COM4,
		CC1, CC2, WS1, PLUS1, MINUS1, STAR1, SLASH1, PCT1, SHARP1,
		CIRC1, GT1, GT2, LT1, LT2, OR1, AND1, ASG1, NOT1, DOTS1,
		Question1, Tildie1,
		MAXSTATE,
	//	S_SELF, S_SELFB, S_EOF, S_NL, S_EOFSTR,
	//	S_STNL, S_COMNL, S_EOFCOM, S_COMMENT, S_EOB, S_WS, S_NAME
	};
	enum ACTION {
		S_SELF, S_SELFB, S_EOF, S_NL, S_EOFSTR,
		S_STNL, S_COMNL, S_EOFCOM, S_COMMENT, S_EOB, S_WS, S_NAME

	};
	
	static constexpr size_t	QBSBIT = 0100;

	using cclass = std::variant<std::nullptr_t, const C_WS, const C_ALPH, const C_NUM, const C_EOF, C_XX, const char>;

	struct ACT {
		const Punc token; const ACTION action;
		constexpr ACT( ACTION action) : token(Punc::Null), action(action) {}
		constexpr ACT( Punc tok,   ACTION act) : token(tok), action(act) {}
	};
	using act_t =  std::variant <const STATE, const  ACT>;


	static constexpr ACTION	GETACT(ACT st) { return st.action; }


	class cclass_t {
		using word_type = uint64_t;
		static constexpr ptrdiff_t _Bits = 256; // only 256 charaters,
		static constexpr ptrdiff_t _Bitsperword = CHAR_BIT * sizeof(word_type);
		static constexpr ptrdiff_t _Words = _Bits == 0 ? 0 : (_Bits - 1) / _Bitsperword; // NB: number of words - 1
		using array_type = std::array<word_type, _Words>;


		static constexpr array_type& _set_unchecked(array_type& array, size_t pos) noexcept { // set bit at _Pos to _Val, no checking
			array[pos / _Bitsperword] |= word_type{ 1 } << (pos % _Bitsperword);
			return array;
		}
		static  constexpr array_type& _reset_unchecked(array_type& array, size_t pos) noexcept { // set bit at _Pos to _Val, no checking
			array[pos / _Bitsperword] &= ~(word_type{ 1 } << (pos % _Bitsperword));
			return array;
		}
		array_type _words;
	public:
		constexpr cclass_t(char c0) :_words() { _set_unchecked(_words, static_cast<size_t>(c0)); }
		constexpr cclass_t(char c0, char c1): cclass_t(c0) { _set_unchecked(_words, static_cast<size_t>(c1)); }
		template<typename T>
		constexpr cclass_t(std::initializer_list<T> list) :_words() {
			for (auto a : list) _set_unchecked(_words, a);
		}
	//	constexpr cclass_t(char c0, char c1,  char c2) : cclass_t(c0,c1) { _set_unchecked(_words, static_cast<size_t>(c2)); }

	};
	template<class TransitionType>
	class finiteAutomata {
	protected:
		int startState;
		int currentState;
		std::set<int> finalStates;
		std::map<int, std::map<TransitionType, int>> transitions;

	public:
		finiteAutomata(int st) : startState(st) {}
		void addFinal(int fs) { finalStates.insert(fs);  }
		void addTransition(int st, TransitionType t, int ns) { transitions[st][t] = ns; }
		void placeInStart() { currentState = startState; }
	
		int inFinal() { return finalStates.count(currentState) > 0; }
		bool advance(TransitionType value) {
			if(transitions.count(currentState)!=0)
				if (transitions[currentState].count(value) != 0) {
					currentState = transitions[currentState][value];
					return true;
				}
			return false;
		}
	};



	 struct	fsm_t {
		const STATE	state;		/* if in this state */
		const cclass_t cc;
	//	const cclass ch[4];		/* and see one of these characters */
		const ACT	nextstate;	/* enter this state if +ve */
		constexpr fsm_t(STATE state, cclass_t cc, ACT s) : state(state), cc(cc), nextstate(s) {}
	};
	static constexpr auto EOFC = -1;
#if 0
	static   fsm_t fsm[]{
		/* start state */
		{ STATE::START,	cclass_t( '\0'  ),	S_SELF } //STATE::S_SELF },

	//	{	START,	{ C_NUM{} },	NUM1  },
		{	START,	{ '.' },	NUM3  },
		//{	START,	{ C_ALPH{} },	ID1  },
		{   START,	{ 'L' },	ST1  },
		{   START,	{ '"' },	ST2  },
		{   START,	{ '\'' },	CC1  },
		{   START,	{ '/' },	COM1  },
		{	START,	{ -1 },	S_EOF  },
		{	START,	{ '\n' },	S_NL  },
		{ 	START,	{ '-' ,'\0'},	MINUS1  }

		{ 	START,	{ '+' },	PLUS1  },
		{ 	START,	{ '<' },	LT1  },
		{ 	START,	{ '>' },	GT1  },
		{ 	START,	{ '=' },	ASG1  },
		{ 	START,	{ '!' },	NOT1  },
		{ 	START,	{ '&' },	AND1  },
		{ 	START,	{ '|' },	OR1  },
		{ 	START,	{ '#' },	SHARP1  },
		{  	START,	{ '%' },	PCT1  },
		{  	START,	{ '[' },	ACT(Punc::LBrace,S_SELF)  },
		{ 	START,	{ ']' },	ACT(Punc::RBrace,S_SELF) },
		{ 	START,	{ '(' },	ACT(Punc::LParen,S_SELF) },
		{ 	START,	{ ')' },	ACT(Punc::RParen,S_SELF) },
		{ 	START,	{ '*' },	STAR1 },
		{ 	START,	{ ',' },	ACT(Punc::Comma,S_SELF) },
		{ 	START,	{ '?' },	ACT(Punc::QuestionMark,S_SELF) },
		{ 	START,	{ ':' },	ACT(Punc::Colon,S_SELF) },
		{ 	START,	{ ';' },	ACT(Punc::LBracket,S_SELF) },
		{ 	START,	{ '{' },	ACT(Punc::RBracket,S_SELF) },
		{ 	START,	{ '}' },	ACT(Punc::LBracket,S_SELF) },
		{ 	START,	{ '~' },	Tildie1 },
		{ 	START,	{ '^' },	CIRC1 },
		{ 	START,	{ '?' },	Question1 },
	};

		{ 	Tildie1,	{ C_XX{} },	ACT(Punc::BitNot,S_SELFB) },
		{ 	Tildie1,	{ '!' },	ACT(Punc::NotEquiv,S_SELF) },
		{   Tildie1,	{ '=' },	ACT(Punc::Equiv,S_SELF) },

		{ 	Question1,	{ C_XX{} },	ACT(Punc::QuestionMark,S_SELFB) },
		{ 	Question1,	{ '.' },	ACT(Punc::SafeDot,S_SELF) },
		{   Question1,	{ ':' },	ACT(Punc::SafeColon,S_SELF) },

			/* saw a digit */
		{ 	NUM1,	{ C_XX{} },	ACT(Punc::Number,S_SELFB) },
		{ 	NUM1,	{ C_NUM{}, C_ALPH{}, '.' },	NUM1 },
		{ 	NUM1,	{ 'E', 'e' },	NUM2 },
		{ 	NUM1,	{ '_' },	ACT(Punc::Number,S_SELFB) },

			/* saw possible start of exponent, digits-e */
		{ 	NUM2,	{ C_XX{} },	ACT(Punc::Number,S_SELFB) },
		{ 	NUM2,	{ '+', '-' },	NUM1 },
		{ 	NUM2,	{ C_NUM{}, C_ALPH{} },	NUM1 },
		{ 	NUM2,	{ '_' },	ACT(Punc::Number,S_SELFB) },

			/* saw a '.', which could be a number or an operator */
		{ 	NUM3,	{ C_XX{} },	ACT(Punc::Dot,S_SELFB) },
		{ 	NUM3,	{ '.' },	DOTS1 },
		{ 	NUM3,	{ C_NUM{} },	NUM1 },

		{ 	DOTS1,	{ C_XX{} },	ACT(Punc::Unkonwn, S_SELFB) },
		{ 	DOTS1,	{ C_NUM{} },	NUM1 },
		{ 	DOTS1,	{ '.' },	ACT(Punc::Ellipsis, S_SELF) },

			/* saw a letter or _ */
		{ 	ID1,	{ C_XX{} },	ACT(Punc::Name,S_NAME) },
		{ 	ID1,	{ C_ALPH{}, C_NUM{} },	ID1 },

			/* saw L (start of wide string?) */
		{ 	ST1,	{ C_XX{} },	ACT(Punc::Name,S_NAME) },
		{ 	ST1,	{ C_ALPH{}, C_NUM{} },	ID1 },
		{ 	ST1,	{ '"' },	ST2 },
		{ 	ST1,	{ '\'' },	CC1 },

			/* saw " beginning string */
		{ 	ST2,	{ C_XX{} },	ST2 },
		{ 	ST2,	{ '"' },	ACT(Punc::String, S_SELF) },
		{ 	ST2,	{ '\\' },	ST3 },
		{ 	ST2,	{ '\n' },	S_STNL },
		{ 	ST2,	{ EOFC },	S_EOFSTR },

			/* saw \ in string */
		{ 	ST3,	{ C_XX{} },	ST2 },
		{ 	ST3,	{ '\n' },	S_STNL },
		{ 	ST3,	{ EOFC},	S_EOFSTR },

			/* saw ' beginning character const */
		{ 	CC1,	{ C_XX{} },	CC1 },
		{ 	CC1,	{ '\'' },	ACT(Punc::Char, S_SELF) },
		{ 	CC1,	{ '\\' },	CC2 },
		{ 	CC1,	{ '\n' },	S_STNL },
		{ 	CC1,	{ EOFC },	S_EOFSTR },

			/* saw \ in ccon */
		{ 	CC2,	{ C_XX{} },	CC1 },
		{ 	CC2,	{ '\n' },	S_STNL },
		{ 	CC2,	{ EOFC },	S_EOFSTR },

			/* saw /, perhaps start of comment */
		{ 	COM1,	{ C_XX{} },	ACT(Punc::Slash, S_SELFB) },
		{ 	COM1,	{ '=' },	ACT(Punc::DivAssign, S_SELF) },
		{ 	COM1,	{ '*' },	COM2 },
		{ 	COM1,	{ '/' },	COM4 },

			/* saw / then *, start of comment */
		{ 	COM2,	{ C_XX{} },	 COM2 },
		{ 	COM2,	{ '\n' },	S_COMNL },
		{ 	COM2,	{ '*' },	COM3 },
		{ 	COM2,	{ EOFC },	S_EOFCOM }, // error

			/* saw the * possibly ending a comment */
		{ 	COM3,	{ C_XX{} },	COM2 },
		{ 	COM3,	{ '\n' },	S_COMNL },
		{ 	COM3,	{ '*' },	COM3 },
		{	COM3,	{ '/' }, ACT(Punc::CommentBlock,S_SELF) }, // S_COMMENT },

			/* // comment */
		{ 	COM4,	{ C_XX{} },	COM4 },
		{ COM4,	{ '\n' },	ACT(Punc::CommentLine,S_SELF) }, //S_NL },
		{ 	COM4,	{ EOFC },	S_EOFCOM }, // error

			/* saw white space, eat it up */
		{ 	WS1,	{ C_XX{} },	S_WS },
		{ 	WS1,	{ ' ', '\t', '\v' },	WS1 },

			/* saw -, check --, -=, -> */
		{ 	MINUS1,	{ C_XX{} },	ACT(Punc::Sub, S_SELFB) },
		{ 	MINUS1,	{ '-' },	ACT(Punc::MinusMinus, S_SELF) },
		{ 	MINUS1,	{ '=' },	ACT(Punc::SubAssign,S_SELF) },

			/* saw +, check ++, += */
		{ 	PLUS1,	{ C_XX{} },	ACT(Punc::Sub, S_SELFB) },
		{ 	PLUS1,	{ '+' },	ACT(Punc::PlusPlus, S_SELF) },
		{ 	PLUS1,	{ '=' },	ACT(Punc::SubAssign, S_SELF) },

			/* saw <, check <<, <<=, <= */
		{ 	LT1,	{ C_XX{} },	ACT(Punc::Less, S_SELFB) },
		{ 	LT1,	{ '<' },	LT2 },
		{ 	LT1,	{ '=' },	ACT(Punc::LessEq, S_SELF) },
		{ 	LT2,	{ C_XX{} },	ACT(Punc::LShift, S_SELFB) },
		{ 	LT2,	{ '=' },	ACT(Punc::LShiftAssign, S_SELF) },

			/* saw >, check >>, >>=, >= */
		{ 	GT1,	{ C_XX{} },	ACT(Punc::Greater, S_SELFB) },
		{ 	GT1,	{ '>' },	GT2 },
		{ 	GT1,	{ '=' },	ACT(Punc::GreaterEq, S_SELF) },
		{ 	GT2,	{ C_XX{} },	ACT(Punc::RShift, S_SELFB) },
		{ 	GT2,	{ '=' },	ACT(Punc::RShiftAssign, S_SELF) },

			/* = */
		{ 	ASG1,	{ C_XX{} },	ACT(Punc::Assign, S_SELFB) },
		{ 	ASG1,	{ '=' },	ACT(Punc::Eq, S_SELF) },

			/* ! */
		{ 	NOT1,	{ C_XX{} },	ACT(Punc::Not, S_SELFB) },
		{ 	NOT1,	{ '=' },	ACT(Punc::NotEq, S_SELF) },

			/* & */
		{ 	AND1,	{ C_XX{} },	ACT(Punc::BitAnd, S_SELFB) },
		{ 	AND1,	{ '&' },	ACT(Punc::And, S_SELF) },
		{ 	AND1,	{ '=' },	ACT(Punc::BitAndAssign, S_SELF) },

			/* | */
		{ 	OR1,	{ C_XX{} },	ACT(Punc::BitOr, S_SELFB) },
		{ 	OR1,	{ '|' },	ACT(Punc::Or, S_SELF) },
		{ 	OR1,	{ '=' },	ACT(Punc::BitOrAssign, S_SELF) },

			/* # */
		{ 	SHARP1,	{ C_XX{} },	ACT(Punc::Hash, S_SELFB) },
		{ 	SHARP1,	{ '#' },	ACT(Punc::TokenPaste, S_SELF) },

			/* % */
		{ 	PCT1,	{ C_XX{} },	ACT(Punc::Mod, S_SELFB) },
		{ 	PCT1,	{ '=' },	ACT(Punc::ModAssign, S_SELF) },

			/* * */
		{ 	STAR1,	{ C_XX{} },	ACT(Punc::Mul, S_SELFB) },
		{ 	STAR1,	{ '=' },	ACT(Punc::MulAssign, S_SELF) },

			/* ^ */
		{ 	CIRC1,	{ C_XX{} },	ACT(Punc::BitXor, S_SELFB) },
		{ 	CIRC1,	{ '=' },	ACT(Punc::BitXorAssign, S_SELF) },

	};
#endif

};
