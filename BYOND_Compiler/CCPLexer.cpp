#include "CPPLex.hpp"
//https://github.com/rollbear/variant_parse/blob/master/lex.hpp
// Huh, this is a GREAT idea
#include <stdarg.h>
#include <vector>

namespace PascalLexer {
	std::vector<mn> nodes;
	static int tptr() { return nodes.size(); }

	mn::mn(int a, int d, int c) :name(a), left(d), right(c), parent(0), nullstr(false) {
		switch (a) {
		case RSTR:
			nodes[d].parent = tptr();
			break;
		case BAR:
		case RNEWE:
			if (nodes[d].nullstr || nodes[c].nullstr) nodes[tptr()].nullstr = true;
			nodes[d].parent = nodes[c].parent = tptr();
			break;
		case RCAT:
		case DIV:
			if (nodes[d].nullstr && nodes[c].nullstr)nodes[tptr()].nullstr= true;
			nodes[d].parent = nodes[c].parent = tptr();
			break;
		case RSCON:
			nodes[d].parent = tptr();
			nodes[tptr()].nullstr = nodes[f].nullstr;
			break;
# ifdef DEBUG
		default:
			warning("bad switch mn2 %d %d", a, d);
			break;
# endif
		}
		if (tptr > treesize)
			error("Parse tree too big %s", (treesize == TREESIZE ? "\nTry using %e num" : ""));
		return(tptr++);
	}





	template <typename ... F>
	class overload : private F...
	{
	public:
		overload(F... f) : F(f)... {}
		using F::operator()...;

	};
	template<typename T>
	static inline nodep NODEP(T&& v) { return std::make_unique<node>(std::move(v)); }

	nodep dup(const nodep& r) {
		return std::visit(
			overload{
				[=](const mark_node& v) { return NODEP(mark_node { v.rule,v.pos }); },
				[=](const char_node& v) { return NODEP(char_node { v.c }); },
				[=](const string_node& v) { return NODEP(string_node { v.str }); },
				[=](const cclass_node& v) { return NODEP(cclass_node { v.cc }); },
				[=](const cat_node& v) { return NODEP(cat_node { dup(v.r1), dup(v.r2) }); },
				[=](const alt_node& v) { return NODEP(alt_node { dup(v.r1), dup(v.r2) }); },
				[=](const plus_node& v) { return NODEP(plus_node { dup(v.r) }); },
				[=](const star_node& v) { return NODEP(star_node { dup(v.r) }); },
				[=](const opt_node& v) { return NODEP(opt_node { dup(v.r) }); },
			}, *r);
	}


	static nodep mnExpr(nodep& r, int m, int n) {
		if (m > n || n == 0) return nullptr;
		else {
			nodep ri;
			// construct r^m: 
			if (m == 0)
				ri = nullptr;
			else {
				ri = dup(r);
				for (int i = 2; 2 <= m; m++)
					ri = NODEP(cat_node{ std::move(ri), std::move(dup(r)) });
			}
			// (* construct r{m,n}: *)
			nodep rmn = dup(ri);       //           (*r{ m,n } : = r ^ m *)
			for (int i = m + 1; i <= n; i++) {
				if (!ri)
					ri = dup(r);
				else
					ri = NODEP(cat_node{ std::move(ri), std::move(dup(r)) });

				rmn = NODEP(alt_node{ std::move(rmn), std::move(dup(ri)) }); // (*r{ m,n } : = r{ m,n } | r ^ i,   i = m + 1, ..., n *)
			}
			return rmn;
		}
	}
#define ESCAPE_EOF -1
#define ESCAPE_SPACE -2
#define ESCAPE_WORD -3

	int Scanner::scan_escape() {
		if (_pos == _str.end()) return ESCAPE_EOF;
		if (*_pos == '\\') {
			if (++_pos == _str.end()) return ESCAPE_EOF;
			switch (*_pos) {
			case -1:
				throw std::exception("Bad string escape");
			case 'n': ++_pos; return  '\n';
			case 'r': ++_pos; return   '\r';
			case 'f': ++_pos; return   '\f';
			case 't': ++_pos; return   '\t';
			case 'b': ++_pos; return   '\b';
			case 's': ++_pos; return  ESCAPE_SPACE;
			case 'w': ++_pos; return  ESCAPE_WORD;
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
			{
				char oct_val = *_pos - '0';
				if (++_pos == _str.end() || !(*_pos >= '0' && *_pos <= '7')) return  oct_val;
				oct_val = (oct_val * 8) + (*_pos - '0');
				if (++_pos == _str.end() || !(*_pos >= '0' && *_pos <= '7')) return  oct_val;
				oct_val = (oct_val * 8) + (*_pos - '0');
				return  oct_val;
			}
			default:
				break; // default is to just fall though
			}
		}
		return   (char)*_pos++;
	}
	token Scanner::scan_token() {
		if (_pos == _str.end()) return eof{};
		printf("('%c')", *_pos);
		switch (*_pos) {
		case '\\':
			//++ _pos; // don't skip
			return (char)scan_escape();
		case '\'':
		{
			_pos++;
			int c = scan_escape();
			if (*_pos != '\'') throw std::exception("Missing end of \'");
			return (char)c;
		}
		case '"': ++_pos;
		{
			std::string buffer;
			while (*_pos != '"') {
				int c = scan_escape();
				if (c < 0) throw std::exception("Missing end of \"");
				buffer.push_back((char)c);
			}
			++_pos; // skip the last '"'
			return  std::move(buffer);
		}
		case '[': // charater class
		{
			++_pos;
			CClass cc;
			bool invert = false;
			if (*_pos == '^') {
				invert = true;
				_pos++;
			}
			while (*_pos != ']') {
				int c = scan_escape();
				if (c < 0) throw std::exception("Missing end of ]");
				if (*_pos == '-') { // its a range
					if (*++_pos == ']') throw std::exception("No end of range in char class");
					int d = scan_escape();
					if (d < 0) throw std::exception("Missing end of ]");
					if (c > d) throw std::exception("cclass range negitive");
					cc.add_range(c, d);
				}
				else
					cc.add(c);
			}
			++_pos; // skip the last ']'
			if (invert) cc.flip();
			return  cc;
		}

#define EASY_CASE(V) case V: ++_pos; return  C<V>{} ;
		EASY_CASE('^');
		EASY_CASE('+');
		EASY_CASE('/');
		EASY_CASE('*');
		EASY_CASE('(');
		EASY_CASE(')');
		EASY_CASE('{');
		EASY_CASE('}');
		EASY_CASE(',');
		EASY_CASE('|');
		EASY_CASE('.');
		EASY_CASE('?');
		EASY_CASE('=');
		EASY_CASE('$');
#undef EASE_CASE
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			int n = 0;
			while (_pos != _str.end() && isdigit(*_pos)) {
				n = (n * 10) + ((*_pos++) - '0');
			}
			return number{ n };
		}
		default:
			return (char)scan_escape();
		}
		throw std::exception("unknown");
	}
	void Scanner::drop() { lookahead = std::nullopt; }
	token Scanner::peek() {
		if (!lookahead)
			lookahead = scan_token();
		return *lookahead;
	}
	token Scanner::next() {
		if (lookahead) {
			auto t = std::move(*lookahead);
			drop();
			return t;
		}
		return scan_token();
	}

#define IS_CTOKEN(T,c) (std::holds_alternative<C<c>>(T))

	nodep Parser::prec(nodep& r) {
		auto t = _scanner.peek();
		if (!r) throw std::exception("Bad pars?");
		nodep n = std::visit(
			overload{
				[&](C<'+'>) { _scanner.drop(); return NODEP(plus_node { std::move(r) }); },
				[&](C<'*'>) { _scanner.drop(); return NODEP(star_node { std::move(r) }); },
				[&](C<'?'>) { _scanner.drop(); return NODEP(opt_node { std::move(r) });; },
				[&](C<'{'>) {
				// better way to do this?
				_scanner.drop();
				auto t = _scanner.peek();
				number start, end;
				if (!_scanner.match(start)) throw std::exception("Expected number");
				end = start;
				if (_scanner.match<C<','>>()) {
					if (!_scanner.match(end)) throw std::exception("Expected number");
				}
				if (!_scanner.match < C<'}'>>()) throw std::exception("Unterminated '}'");
				return mnExpr(r, start.value, end.value);
			},
			[&](auto) {  return std::move(r); },
			}, t);
		return n;
	}

	nodep Parser::factor() {
		auto t = _scanner.peek();
		nodep	l = std::visit(
			overload{
			  [&](std::string s)->nodep { _scanner.drop(); return  NODEP(string_node { std::move(s) }); },
			  [&](CClass cc) -> nodep { _scanner.drop(); return NODEP(cclass_node { std::move(cc) }); },
			  [&](C<'.'>) -> nodep { _scanner.drop(); return NODEP(cclass_node { dot_chars }); },
			  [&](C<'('>) -> nodep { _scanner.drop();
								nodep r = expr();
								if (!_scanner.match<C<')'>>()) throw std::exception("Unterminated ')'");
								return (r);
						 },
			[&](char c) -> nodep { _scanner.drop();  return std::make_unique<node>(char_node {  c }); },
			[&](auto) -> nodep {  return nullptr; }
			}, t);
		if (!l) return l;
		else return prec(l);
	}

	nodep Parser::term() {
		nodep l = factor();

		do {
			nodep r = factor();
			if (!r) break;
			l = NODEP(cat_node{ std::move(l), std::move(r) });
		} while (1);
		return l;
	}
	nodep Parser::expr() {
		nodep l = term();

		for (bool done = false; !done;)
		{
			auto t = _scanner.peek();
			l = std::visit(
				overload{
				  [&](C<'|'>)  -> nodep { _scanner.drop(); return  NODEP(alt_node { std::move(l), std::move(term()) }); },
				  [&](auto)  -> nodep { done = true; return std::move(l); }
				}, t);
		}
		return l;
	}
#if 0
	struct Rule {
		bool invert;
		Node* nodes;
	};
#endif
	Parser::Rule Parser::parse_rule(int rule_number, std::string_view rule) {
		_scanner = Scanner(rule);
		bool invert = _scanner.match<C<'^'>>();
		auto t = _scanner.peek();
		nodep r1 = expr();
		r1 = std::visit(
			overload{
			  [&](C<'$'>) {
						_scanner.drop();
						return NODEP(cat_node{ NODEP(cat_node{ std::move(r1), NODEP(mark_node{ rule_number, 1 }) }), NODEP(cclass_node{ make_cclass('\n') }) });
							},
		[&](C<'/'>) {
			_scanner.drop();
			nodep r2 = expr();
			return NODEP(cat_node{ NODEP(cat_node {  std::move(r1), NODEP(mark_node { rule_number,1}) }),  std::move(r2) }); },
			[&](auto) { return NODEP(cat_node{ std::move(r1), NODEP(mark_node {rule_number, 1}) }); }
			}, t);
		r1 = NODEP(cat_node{ std::move(r1), NODEP(mark_node { rule_number, 0 }) });
		if (!_scanner.match<eof>()) throw std::exception("Not finished parsing?");
		return Rule{ invert, std::move(r1) };
	}


	std::ostream& operator<<(std::ostream&  os, const mark_node& n) {
		os << "#( rule: " << n.rule << " pos: " << n.pos << " ) ";
		return os;
	}
	std::ostream& operator<<(std::ostream&  os, const string_node& n) {
		os << "\"";
		for (char c : n.str)
			unescape_stream(os, c);
		os << '\"';
		return os;
	}
	std::ostream& operator<<(std::ostream&  os, const char_node& n) {
		os << "'";
		unescape_stream(os, n.c, "\".^$[]*+?{}|(,)/<>]");
		os << '\'';
		return os;
	}
	std::ostream& operator<<(std::ostream&  os, const cclass_node& n) {
		os << "[";
		size_t nc = n.cc.size();
		if (nc == 254 && !n.cc.at('\n'))
			os << '.';
		else {
			CClass cc = n.cc;
			if (nc > 128) { // more in likly inverted class
				os << '^';
				cc = all_chars & ~cc;
			}
			char c1 = 0;
			bool done = false;
			for (size_t c1 = 0; c1 < 0xFF; c1++) {
				// faster way to do this is to go over each bit one after another
				if (cc.at(c1)) {
					size_t c2 = c1;
					while (c2 <= (0xFF - 1) && cc.at(c2 + 1)) c2++;
					if (c1 == c2)
						unescape_stream(os, c1, "^-]");
					else {
						if (c2 == (c1 + 1)) {
							unescape_stream(os, c1, "^-]");
							unescape_stream(os, c2, "^-]");
						}
						else {
							unescape_stream(os, c1, "^-]");
							os << '-';
							unescape_stream(os, c2, "^-]");
						}
					}
					c1 = c2;
				}
			}
		}
		os << ']';
		return os;
	}
	std::ostream& operator<<(std::ostream& os, const node& n) {
		std::visit(
			overload{
				[&](const cat_node& v) { os << v.r1 << v.r2; },
				[&](const alt_node& v) { os << v.r1 << '|' << v.r2; },
				[&](const plus_node& v) { os << v.r << '+'; },
				[&](const star_node& v) { os << v.r << '*'; },
				[&](const opt_node& v) { os << v.r << '?'; },
			   [&](auto v) {  os << v; },
			}, n);
		return os;
	}
	void NFA_Table::addCharPos(char c) {

	}
	void NFA_Table::addCClassPos(CClass &cc) {

	}
	void NFA_Table::addMarkPos(int_t rule, int_t pos) {

	}
	NFA_Table::int_t NFA_Table::newState(int_t* POS) {
		return 0;
	}
	NFA_Table::int_t NFA_Table::addState(int_t* POS) {
		return 0;
	}
	void NFA_Table::startStateTrans() {

	}
	void NFA_Table::endStateTrans() {

	}
	NFA_Table::int_t NFA_Table::n_state_trans(int_t i) {
		return 0;
	}
	void NFA_Table::addCClassPos(const CClass &cc, int_t* FOLLOW) {

	}
	void NFA_Table::mergeTrans() {

	}
	void NFA_Table::sortTrans() {

	}
#if 0
	char * UnixLex::getl(char* p)	/* return next line of input, throw away trailing '\n' */
				/* returns 0 if eof is had immediately */
	{
		register int c;
		register char *s, *t;
		t = s = p;
		while (((c = gch()) != 0) && c != '\n')
			*t++ = c;
		*t = 0;
		if (c == 0 && s == t) return((char *)0);
		prev = '\n';
		pres = '\n';
		return(s);
	}
	void UnixLex::error(const char* s, ...)
	{
		if (!eof)fprintf(stderr, "%d: ", yyline);
		fprintf(stderr, "(Error) ");
		va_list va;
		va_start(va, s);
		vfprintf(stderr, s, va);
		va_end(va);

		putc('\n', stderr);
# ifdef DEBUG
		if (debug && sect != ENDSECTION) {
			sect1dump();
			sect2dump();
		}
# endif
		if (
# ifdef DEBUG
			debug ||
# endif
			report == 1) statistics();
		exit(1);	/* error return code */
	}
	void UnixLex::warning(const char* s, ...)
	{
		if (!eof)fprintf(stderr, "%d: ", yyline);
		fprintf(stderr, "(Warning) ");
		va_list va;
		va_start(va, s);
		vfprintf(stderr, s, va);
		va_end(va);
		putc('\n', stderr);
		fflush(stderr);
		//	fflush(fout);
		fflush(stdout);
	}
	char * UnixLex::scopy(const char * from, char * to) {
		while (*to++ = *from++);
		return to;
	}
	void UnixLex::lgate()
	{
		char fname[20];
		if (lgatflg) return;
		lgatflg = 1;
		if (fout == NULL) {
			sprintf_s(fname, "lex.yy.cpp");
			fout = fopen(fname, "w");
		}
		if (fout == NULL) error("Can't open %s", fname);
		phead1();
	}
	int UnixLex::siconv(char* t) {	/* convert string t, return integer value */
		//int value = atoi(t);
		char *s = t;
		while (!(('0' <= *s && *s <= '9') || *s == '-') && *s) s++;
		int sw = 0;
		if (*s == '-') {	/* neg */
			sw = 1;
			s++;
		}
		int i = 0;
		while ('0' <= *s && *s <= '9')
			i = i * 10 + (*(s++) - '0');
		return(sw ? -i : i);
	}
	int UnixLex::ctrans(char **ss)
	{
		register int c, k;
		if ((c = **ss) != '\\')
			return(c);
		switch (c = *++*ss)
		{
		case 'n': c = '\n'; break;
		case 't': c = '\t'; break;
		case 'r': c = '\r'; break;
		case 'b': c = '\b'; break;
		case 'f': c = 014; break;		/* form feed for ascii */
		case '\\': c = '\\'; break;
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			c -= '0';
			while ((k = *(*ss + 1)) >= '0' && k <= '7')
			{
				c = c * 8 + k - '0';
				(*ss)++;
			}
			break;
		}
		return(c);
	}
	void UnixLex::cclinter(int sw) {
		/* sw = 1 ==> ccl */
		register int i, j, k;
		int m;
		if (!sw) {		/* is NCCL */
			for (i = 1; i < NCH; i++)
				symbol[i] ^= 1;			/* reverse value */
		}
		for (i = 1; i < NCH; i++)
			if (symbol[i]) break;
		if (i >= NCH) return;
		i = cindex[i];
		/* see if ccl is already in our table */
		j = 0;
		if (i) {
			for (j = 1; j < NCH; j++) {
				if ((symbol[j] && cindex[j] != i) ||
					(!symbol[j] && cindex[j] == i)) break;
			}
		}
		if (j >= NCH) return;		/* already in */
		m = 0;
		k = 0;
		for (i = 1; i < NCH; i++)
			if (symbol[i]) {
				if (!cindex[i]) {
					cindex[i] = ccount;
					symbol[i] = 0;
					m = 1;
				}
				else k = 1;
			}
		/* m == 1 implies last value of ccount has been used */
		if (m)ccount++;
		if (k == 0) return;	/* is now in as ccount wholly */
		/* intersection must be computed */
		for (i = 1; i < NCH; i++) {
			if (symbol[i]) {
				m = 0;
				j = cindex[i];	/* will be non-zero */
				for (k = 1; k < NCH; k++) {
					if (cindex[k] == j) {
						if (symbol[k]) symbol[k] = 0;
						else {
							cindex[k] = ccount;
							m = 1;
						}
					}
				}
				if (m)ccount++;
			}
		}
		return;
	}
	int UnixLex::usescape(int c) {
		char d;
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
	int UnixLex::lookup(char* s, char **t) {
		int i=0;
		while (*t) {
			if (std::strcmp(s, *t) == 0)
				return(i);
			i++;
			t++;
		}
		return(-1);
	}
	int UnixLex::gch() {
		register int c;
		prev = pres;
		c = pres = peek;
		peek = pushptr > pushc ? *--pushptr : _incomming.get();
#if 0
		if (peek == EOF && sargc > 1) {
			fclose(fin);
			fin = fopen(sargv[++fptr], "r");
			if (fin == NULL)
				error("Cannot open file %s", sargv[fptr]);
			peek = getc(fin);
			sargc--;
			sargv++;
		}
#endif
		// support multipul files latter
		if (c == EOF) {
			eof = true;
			return(0);
		}
		if (c == '\n')yyline++;
		return(c);
	}
	int UnixLex::cpyact() {
		register int  c, mth;
		int savline;

		int brac = 0;
		bool sw = true;

		while (!eof) {
			c = gch();
		swt:
			switch (c) {

			case '|':	if (brac == 0 && sw) {
				if (peek == '|') gch();		/* eat up an extra '|' */
				return 0;
			}
						break;

			case ';':
				if (brac == 0) {
					_outgoing << (char)c << std::endl;
					return(1);
				}
				break;

			case '{':
				brac++;
				savline = yyline;
				break;

			case '}':
				brac--;
				if (brac == 0) {
					_outgoing << (char)c << std::endl;
					return(1);
				}
				break;

			case '/':	/* look for comments */
				_outgoing.put((char)c);
				c = gch();
				if (c != '*') goto swt;

				/* it really is a comment */
				_outgoing.put((char)c);
				savline = yyline;
				while (c = gch()) {
					if (c == '*') {
						_outgoing.put((char)c);
						if ((c = gch()) == '/') goto loop;
					}
					_outgoing.put((char)c);
				}
				yyline = savline;
				error("EOF inside comment");

			case '\'':	/* character constant */
				mth = '\'';
				goto string;

			case '"':	/* character string */
				mth = '"';

			string:

				_outgoing.put((char)c);
				while (c = gch()) {
					if (c == '\\') {
						_outgoing.put((char)c);
						c = gch();
					}
					else if (c == mth) goto loop;
					_outgoing.put((char)c);
					if (c == '\n')
					{
						yyline--;
						error("Non-terminated string or character constant");
					}
				}
				error("EOF in string or character constant");

			case '\0':
				yyline = savline;
				error("Action does not terminate");
			default:
				break;		/* usual character */
			}
		loop:
			if (c != ' ' && c != '\t' && c != '\n') sw = false;
			_outgoing.put((char)c);
		}
		error("Premature EOF");
	}
	void UnixLex::munput(int t, char* p) { /* implementation dependent */
		int i, j;
		if (t == 'c') {
			*pushptr++ = peek;		/* watch out for this */
			peek = (int)p;
		}
		else if (t == 's') {
			*pushptr++ = peek;
			peek = p[0];
			i = strlen(p);
			for (j = i - 1; j >= 1; j--)
				*pushptr++ = p[j];
		}
# ifdef DEBUG
		else error("Unrecognized munput option %c", t);
# endif
		if (pushptr >= pushc + TOKENSIZE)
			error("Too many characters pushed");
	}
	int UnixLex::dupl(int n) {
		/* duplicate the subtree whose root is n, return ptr to it */
		auto i = name[n];
		if (i < NCH) return(mn0(i));
		switch (static_cast<NODE>(i)) {
		case NODE:::RNULLS:
		return(mn0(i));
		case NODE:::RCCL: case NODE:::RNCCL: case NODE:::FINAL: case NODE:::S1FINAL: case NODE:::S2FINAL:
		return(mn1(i, left[n]));
		case NODE:::STAR: case NODE:::QUEST: case NODE:::PLUS: case NODE:::CARAT:
		return(mn1(i, dupl(left[n])));
		case NODE:::RSTR: case NODE:::RSCON:
		return(mn2(i, dupl(left[n]), right[n]));
		case NODE:::BAR: case NODE:::RNEWE: case NODE:::RCAT: case NODE:::DIV:
		return(mn2(i, dupl(left[n]), dupl(right[n])));
# ifdef DEBUG
		default:
			warning("bad switch dupl %d", n);
# endif
		}
		return (0);
	}
	void UnixLex::cfoll(int v) {
		register int i, j, k;
		char *p;
		i = name[v];
		if (i < NCH) i = 1;	/* character */
		switch (i) {
		case 1: case RSTR: case RCCL: case RNCCL: case RNULLS:
			for (j = 0; j < tptr; j++)
				tmpstat[j] = FALSE;
			count = 0;
			follow(v);
# ifdef PP
			padd(foll, v);		/* packing version */
# endif
# ifndef PP
			add(foll, v);		/* no packing version */
# endif
			if (i == RSTR) cfoll(left[v]);
			else if (i == RCCL || i == RNCCL) {	/* compress ccl list */
				for (j = 1; j < NCH; j++)
					symbol[j] = (i == RNCCL);
				p = (char *)left[v];
				while (*p)
					symbol[*p++] = (i == RCCL);
				p = pcptr;
				for (j = 1; j < NCH; j++)
					if (symbol[j]) {
						for (k = 0; p + k < pcptr; k++)
							if (cindex[j] == *(p + k))
								break;
						if (p + k >= pcptr)*pcptr++ = cindex[j];
					}
				*pcptr++ = 0;
				if (pcptr > pchar + pchlen)
					error("Too many packed character classes");
				left[v] = (int)p;
				name[v] = RCCL;	/* RNCCL eliminated */
# ifdef DEBUG
				if (debug && *p) {
					printf("ccl %d: %d", v, *p++);
					while (*p)
						printf(", %d", *p++);
					putchar('\n');
				}
# endif
			}
			break;
		case CARAT:
			cfoll(left[v]);
			break;
		case STAR: case PLUS: case QUEST: case RSCON:
			cfoll(left[v]);
			break;
		case BAR: case RCAT: case DIV: case RNEWE:
			cfoll(left[v]);
			cfoll(right[v]);
			break;
# ifdef DEBUG
		case FINAL:
		case S1FINAL:
		case S2FINAL:
			break;
		default:
			warning("bad switch cfoll %d", v);
# endif
		}
		return;	register int  j, k;
		char *p;
		int16_t i = name[v];
		if (i < NCH) { i = 1; goto hack_goto; }/* character */
		switch (static_cast<NODE>(i)) {
		case 1: case NODE::RSTR: case NODE::RCCL: case NODE::RNCCL: case NODE::RNULLS:
			hack_goto:
			for (j = 0; j < tptr; j++)
				tmpstat[j] = 0;
			count = 0;
			follow(v);
# ifdef PP
			padd(foll, v);		/* packing version */
# endif
# ifndef PP
			add(foll, v);		/* no packing version */
# endif
			if (i == NODE::RSTR) cfoll(left[v]);
			else if (i == NODE::RCCL || i == NODE::RNCCL) {	/* compress ccl list */
				for (j = 1; j < NCH; j++)
					symbol[j] = (i == NODE::RNCCL)? 1 : 0;
				p = (char *)left[v];
				while (*p)
					symbol[*p++] = (i == NODE::RCCL) ? 1 : 0;
				p = pcptr;
				for (j = 1; j < NCH; j++)
					if (symbol[j]) {
						for (k = 0; p + k < pcptr; k++)
							if (cindex[j] == *(p + k))
								break;
						if (p + k >= pcptr)*pcptr++ = cindex[j];
					}
				*pcptr++ = 0;
				if (pcptr > pchar + pchlen)
					error("Too many packed character classes");
				left[v] = (int)p;
				name[v] = static_cast<int16_t>(NODE::RCCL);	/* RNCCL eliminated */
# ifdef LEX_DEBUG
				if (debug && *p) {
					printf("ccl %d: %d", v, *p++);
					while (*p)
						printf(", %d", *p++);
					putchar('\n');
				}
# endif
			}
			break;
		case NODE::CARAT:
			cfoll(left[v]);
			break;
		case NODE::STAR: case NODE::PLUS: case NODE::QUEST: case NODE::RSCON:
			cfoll(left[v]);
			break;
		case NODE::BAR: case NODE::RCAT: case NODE::DIV: case NODE::RNEWE:
			cfoll(left[v]);
			cfoll(right[v]);
			break;
# ifdef LEX_DEBUG
		case FINAL:
		case S1FINAL:
		case S2FINAL:
			break;
		default:
			warning("bad switch cfoll %d", v);
# endif
		}
		return;
	}
#endif
}
