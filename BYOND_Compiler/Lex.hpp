#pragma once


#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <fstream>
#include <queue>
#include <optional>
#include <functional>
#include <variant>
#include <memory>
#include <string>
#include <string_view>
#include <set>
#include <type_traits>

class OldLex {
	constexpr static int CWIDTH = 8;
	constexpr static int CMASK = 0377;
	constexpr static int NCH = 128;
	constexpr static int ctable[2 * NCH] = {
0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
100,101,102,103,104,105,106,107,108,109,
110,111,112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127 };

	constexpr static int  TOKENSIZE = 1000;
	constexpr static int DEFSIZE = 40;
	constexpr static int DEFCHAR = 1000;
	constexpr static int STARTCHAR = 100;
	constexpr static int STARTSIZE = 256;
	constexpr static int CCLSIZE = 1000;
	constexpr static int TREESIZE = 1000;
	constexpr static int NSTATES = 500;
	constexpr static int MAXPOS = 2500;
	constexpr static int NTRANS = 2000;
	constexpr static int NOUTPUT = 3000;
	constexpr static int NACTIONS = 100;
	constexpr static int ALITTLEEXTRA = 30;

	enum Section {
		DEFSECTION = 1,
		RULESECTION = 2,
		ENDSECTION = 5,
	};
	constexpr static int  PC = 1;
	constexpr static int  PS = 1;

	int ccount = 1;
	int casecount = 1;
	int aptr = 1;
	int nstates = NSTATES, maxpos = MAXPOS;
	int treesize = TREESIZE, ntrans = NTRANS;
	int yytop;
	int outsize = NOUTPUT;
	int sptr = 1;
	bool optim = true;
	int report = 2;
	char buf[520];
	int ratfor;		/* 1 = ratfor, 0 = C */
	int yyline;		/* line number of file */
	bool eof;
	bool lgatflg;
	int divflg;
	int funcflag;
	int pflag;
	bool chset;	/* 1 = char set modified */
	using char_t = char;
	using cclass_t = const std::shared_ptr<std::set<char_t>>;
	using string_t = const std::shared_ptr<std::string>;

	enum TYPE {
		CCHAR = -1,
		RCCL = NCH + 90,
		RNCCL,
		RSTR,
		RSCON,
		BAR,
		RNEWE,
		DIV,
		RCAT,
		STAR,
		QUEST,
		PLUS,
		RNULLS,
		CARAT,
		S2FINAL,
		FINAL,
		S1FINAL,
		EMPTY
	};
	


	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

	class node : std::enable_shared_from_this<node> {
	public:
		using pointer = std::shared_ptr<node>;
		struct LEAF { pointer left; };
		struct BRANCH : LEAF { pointer right; };
		struct SLEAF : LEAF { string_t str; };
		using value_t = std::variant<std::monostate, char_t, cclass_t, LEAF, BRANCH, SLEAF>;

		template<typename T, typename ...P>
		static pointer create(P...p) { return std::make_shared<node>(T{ std::forward<P>(p)... }); }
	private:
		TYPE _type;
		value_t _value;
		pointer _parent;
		bool _nullstr;
		bool tempstat;
		node(TYPE type, value_t&& v, pointer p, bool nullstr) : _parent(p), tempstat(false), _nullstr(nullstr), _value(std::move(v)) {}
		void setParent(pointer p) { _parent = p; }
	public:
		node() : _parent(nullptr), tempstat(false), _nullstr(false), _value(std::monostate{}) {}

		const value_t& value() const { return _value; }
		value_t& value() { return _value; }
		operator const value_t& () const { return _value; }
		operator value_t& () { return _value; }
		/// creators
		// ok lets try this
		const pointer& parent() const { return _parent; }
		bool nullstr() const { return _nullstr; }
		const string_t str() const {
			return std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_base_of_v<T, SLEAF>)
					return arg.str;
				else
					return nullptr;
				}, _value);
		}
		const pointer right() const {
			return std::visit([](auto&& arg) -> const pointer {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, BRANCH>)
					return arg.right;
				else
					static_assert(always_false<T>::value, "non-exhaustive visitor!");
				}, _value);
		}
		const pointer left() const {
			return std::visit([](auto&& arg) -> const pointer {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_base_of_v<T, LEAF>)
					return arg.right;
				else
					static_assert(always_false<T>::value, "non-exhaustive visitor!");
				//return nullptr;
				}, _value);
		}
		static pointer create(TYPE t, pointer l, pointer r, pointer p = nullptr) {
			switch (t) {
#if 0
			case TYPE::RSTR:
				l->setParent(p);
				return std::make_shared<node>(t, BRANCH{ l, r }, nullptr, false);
			case TYPE::RSCON:
				l->setParent(p);
				return std::make_shared<node>(t, BRANCH{ l, r }, nullptr, l->nullstr());
#endif
			case TYPE::BAR: case TYPE::RNEWE:
				l->setParent(p); r->setParent(p);
				return std::make_shared<node>(t, BRANCH{ l, r }, nullptr, l->nullstr() || r->nullstr());
			case TYPE::RCAT: case TYPE::DIV:
				l->setParent(p); r->setParent(p);
				return std::make_shared<node>(t, BRANCH{ l, r }, nullptr, l->nullstr() && r->nullstr());
			default:
				throw 0;
			}
		}
		static pointer create(TYPE t, pointer l, string_t r, pointer p = nullptr) {
			switch (t) {
			case TYPE::RSTR:
				l->setParent(p);
				return std::make_shared<node>(t, SLEAF{ l, r }, nullptr, false);
			case TYPE::RSCON:
				l->setParent(p);
				return std::make_shared<node>(t, SLEAF{ l, r }, nullptr, l->nullstr());
			default:
				throw 0;
			}
		}
		static pointer create(TYPE t, cclass_t cc, pointer p = nullptr) {
			switch (t) {
			case TYPE::RCCL:case TYPE::RNCCL:
				return std::make_shared<node>(t, cc, nullptr, cc->empty());
			default:
				throw 0;
			}
		}
		static pointer create(TYPE t, pointer l, pointer p=nullptr) {
			switch (t) {
			case TYPE::STAR:case TYPE::QUEST:
				l->setParent(p);
				return std::make_shared<node>(t, LEAF{ l }, nullptr, true);
			case TYPE::PLUS:case TYPE::CARAT:
				l->setParent(p);
				return std::make_shared<node>(t, LEAF{ l }, nullptr, l->nullstr());
			case TYPE::S2FINAL:
				return std::make_shared<node>(t, std::monostate{}, nullptr, true);
				break;
# ifdef _DEBUG
			case TYPE::FINAL:
			case TYPE::S1FINAL:
				return std::make_shared<node>(t, std::monostate{}, nullptr, false);
				break;
# endif
			default:
				throw 0;
			}
		}
		static pointer create(TYPE t, char_t c, pointer p=nullptr) {
			return std::make_shared<node>(t, c, nullptr, t == TYPE::RNULLS ? true : false);
		}



		void treedump(size_t ident = 0);


		template<typename T>
		constexpr bool is() const { return std::holds_alternative<T>(_value); }

		pointer dup() const {
			switch (_type) {
			case TYPE::RNULLS: case TYPE::CCHAR:
				return create(_type, std::get<char_t>(_value));
			case TYPE::RCCL: case TYPE::RNCCL: 
				return create(_type, std::get<cclass_t>(_value));
			case TYPE::FINAL: case TYPE::S1FINAL: case TYPE::S2FINAL:
				return create(_type, left());
			case TYPE::STAR: case TYPE::QUEST: case TYPE::PLUS:case TYPE::CARAT:
				return create(_type, left()->dup());
			case TYPE::RSTR: case TYPE::RSCON:
				return create(_type, left()->dup(),str());
			case TYPE::BAR: case TYPE::RNEWE: case TYPE::RCAT:case TYPE::DIV:
				return create(_type, left()->dup(), right()->dup());
			default:
				throw 0;
			}
		}

	};
	using node_t = node::pointer;

	void first(node_t n);
	void follow(node_t n);

	static constexpr size_t  LINESIZE = 110;
	int yydebug;
	int debug;		/* 1 = on */
	int charc;
	std::ostream* debugout;


	node::pointer tptr;
	char slist[STARTSIZE];
	char** def, ** subs, * dchar;
	char** sname, * schar;
	char* ccl;
	char* ccptr;
	char* dp, * sp;
	int dptr;
	char* bptr;		/* store input position */
	char* tmpstat;
	int count;
	int** foll;
	int* nxtpos;
	int* positions;
	int* gotof;
	int* nexts;
	char* nchar;
	int** state;
	int* sfall;		/* fallback state num */
	char* cpackflg;		/* true if state has been character packed */
	int* atable;
	int nptr;
	char symbol[NCH];
	char cindex[NCH];
	int xstate;
	int stnum;
	char match[NCH];
	char extra[NACTIONS];
	char* pchar, * pcptr;
	int pchlen = TOKENSIZE;
	long rcount;
	int* verify, * advance, * stoff;
	int scon;
	char* psave;


	// header
	void statistics();
	void tail();
	void head2();
	void head1();
	// string get
	int	ZCH = NCH;
	std::queue<std::string> filelist;
	int	sect = DEFSECTION;
	int	prev = '\n';	/* previous input character */
	int	pres = '\n';	/* present input character */
	int	peek = '\n';	/* next input character */
	std::queue<int> unputq;
	char* slptr = slist;
	std::ostream* fout;
	std::ostream* errorf;
	std::istream* fin;
	std::istream* fother;


	void munput(int c) { unputq.push(c); }
	void munput(const std::string_view s) { for (const auto& c : s) munput(c); }

	void cpyact();  /* copy C action to the next ; or closing } */
	int gch();
	int yylex();
	bool getl(std::string& p);

	void error(const char* s, ...);
	void warning(const char* s, ...);
	void lgate();
	void cclinter(bool noinvert);
	inline static void ident_line(size_t len = 0) { while (len--) putchar(' '); }

	static void allprint(int c);
	inline static void allprint(const std::string_view& s, size_t ident = 0) { ident_line(ident);  for (const auto& c : s) allprint(static_cast<int>(c)); }

	void sect1dump();
	void sect2dump();



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