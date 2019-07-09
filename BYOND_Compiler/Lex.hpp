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

	using cclass_t = std::shared_ptr<std::set<int>>;
	using string_t = std::shared_ptr<std::string>;
	struct node;
	using node_t = std::shared_ptr<node> ;

	struct RCCL { cclass_t cc; };
	struct RNCCL { cclass_t cc; };
	struct RSTR { node_t left;  string_t str;  };
	struct RSCON { node_t left;  string_t str; };

	struct BAR { node_t left;  node_t right; };
	struct RNEWE { node_t left;  node_t right; };
	struct DIV { node_t left;  node_t right; };
	struct RCAT { node_t left;  node_t right; };
	struct STAR { node_t left; };
	struct QUEST { node_t left; };
	struct PLUS { node_t left; };
	struct CARAT { node_t left; };
	struct S2FINAL {  };
	struct FINAL {  };
	struct S1FINAL {  };
#if 0
		RNCCL,

		RSTR,
		RSCON,
		RNEWE,
		FINAL,
		RNULLS,
		RCAT,
		STAR,
		PLUS,
		QUEST,
		DIV,
		BAR,
		CARAT,
		S1FINAL,
		S2FINAL,
#endif
	struct node : public std::enable_shared_from_this<node> {
		using nodep = std::shared_ptr<node>;
		int name;
		std::variant<std::string_view, nodep, nullptr_t> _left;
		std::variant<std::string_view, nodep,nullptr_t> _right;
		nodep parent;
		bool nullstr;
		nodep& right() { return std::get<nodep>(_right); }
		nodep& left() { return std::get<nodep>(_left); }
		const nodep& right() const { return std::get<nodep>(_right); }
		const nodep& left() const { return std::get<nodep>(_left); }
		const std::string_view& lstr() const { return std::get<std::string_view>(_right); }
		const std::string_view& rstr() const { return std::get<std::string_view>(_left); }

		node(int n, nodep l, std::string_view  r);
		node(int n, nodep l, nodep r);
		node(int n, nodep l);
		node(int n, std::string_view l);
		node(int n);
		nodep dup() const;
		node(int n, node* l, node* r) : node(n, l->shared_from_this(), r->shared_from_this()) {}
		node(int n, node* l) : node(n, l->shared_from_this()) {}
	};
	using nodep = node::nodep;

# ifdef _DEBUG
	static constexpr size_t  LINESIZE = 110;
	int yydebug;
	int debug;		/* 1 = on */
	int charc;
	std::ostream* debugout;
	template<typename T>
	inline T freturn(T&& s) { return std::move(s); }
	void ident_line(size_t len = 0) { if (len) for (size_t i = 0; i < len; i++) putchar(' '); }
	void allprint(int c);
	void allprint(const std::string_view& s, size_t ident = 0) { ident_line(ident);  for (const auto& c : s) allprint(static_cast<int>(c)); }

	void sect1dump();
	void sect2dump();
	void treedump();
	void treedump(node::nodep t, size_t ident = 0);
#else
	template<typename T>
	inline T freturn(T&& s) { return std::move(s); }
#endif

	std::shared_ptr<node> tptr;
	char slist[STARTSIZE];
	char** def, **subs, *dchar;
	char** sname, *schar;
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