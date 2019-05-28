#include "CPPLex.hpp"
//https://github.com/rollbear/variant_parse/blob/master/lex.hpp
// Huh, this is a GREAT idea

namespace PascalLexer {
	
	static Node* mnExpr(Node* r, int m, int n) {
		if (m > n || n == 0) return nullptr;
		else {
			Node* ri;
			// construct r^m: 
			if (m == 0)
				ri = nullptr;
			else {
				ri = r;
				for (int i = 2; 2 <= m; m++)
					ri = new CatNode(ri, r);
			}
			// (* construct r{m,n}: *)
			Node*  rmn = ri;       //           (*r{ m,n } : = r ^ m *)
			for (int i = m + 1; i <= n; i++) {
				if (ri == nullptr)
					ri = r;
				else
					ri = new CatNode(ri, r);
				rmn = new AltNode(rmn, ri); // (*r{ m,n } : = r{ m,n } | r ^ i,   i = m + 1, ..., n *)
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
			case 'n': ++_pos; return  '\n' ;
			case 'r': ++_pos; return   '\r' ;
			case 'f': ++_pos; return   '\f' ;
			case 't': ++_pos; return   '\t' ;
			case 'b': ++_pos; return   '\b' ;
			case 's': ++_pos; return  ESCAPE_SPACE;
			case 'w': ++_pos; return  ESCAPE_WORD;
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
			{
				char oct_val = *_pos - '0';
				if (++_pos == _str.end() || !(*_pos >= '0' && *_pos <= '7')) return  oct_val ;
				oct_val = (oct_val * 8) + (*_pos - '0');
				if (++_pos == _str.end() || !(*_pos >= '0' && *_pos <= '7')) return  oct_val ;
				oct_val = (oct_val * 8) + (*_pos - '0');
				return  oct_val ;
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
					add_range_to_cclass(cc, (size_t)c, (size_t)d);
				}
				else
					add_char_to_cclass(cc, c);
			}
			++_pos; // skip the last ']'
			if (invert) flip_cclass(cc);
			return  cc ;
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
	template <typename ... F>
	class overload : private F...
	{
	public:
		overload(F... f) : F(f)... {}
		using F::operator()...;

	};

#define IS_CTOKEN(T,c) (std::holds_alternative<C<c>>(T))

	Node* Parser::prec(Node* r) {
			auto t = _scanner.peek();
			if (r == nullptr) throw std::exception("Bad pars?");
			r= std::visit(

				overload{
					[=](C<'+'>) -> Node*{ _scanner.drop(); return new PlusNode(r); },
					[=](C<'*'>) -> Node* { _scanner.drop(); return new StarNode(r); },
					[=](C<'?'>) -> Node* { _scanner.drop(); return new OptNode(r); },
					[=](C<'{'>) -> Node* {
						// better way to do this?
						_scanner.drop();
						auto t = _scanner.peek();
						number start, end;
						if (!_scanner.match(start)) throw std::exception("Expected number");
						end = start;
						if ( _scanner.match<C<','>>() ) {
							if (!_scanner.match(end)) throw std::exception("Expected number");
						}
						if (!_scanner.match < C<'}'>>()) throw std::exception("Unterminated '}'");
						return mnExpr(r, start.value, end.value);
					},
					[=](auto) -> Node* {  return r; },
					}, t);
		return r;
	}
	
	Node* Parser::factor() {
			auto t = _scanner.peek();
			Node* 	l = std::visit(
				overload{
				  [=](std::string s)-> Node* { _scanner.drop(); return  new StringNode(std::move(s)); },
				  [=](CClass cc) -> Node* { _scanner.drop(); return  new CClassNode(std::move(cc)); },
				  [=](C<'.'>) -> Node* { _scanner.drop(); return  new CClassNode(make_cclass(crange(0, 255))); },
				  [=](C<'('>) -> Node* { _scanner.drop();
									auto r = expr();
									if (!_scanner.match<C<')'>>()) throw std::exception("Unterminated ')'");
									return (r);
							 },
				[=](char c) -> Node* { _scanner.drop();  return new  CharNode( c); },
				[&](auto) -> Node* {  return nullptr; }
				}, t);
		return l == nullptr? l : prec(l);
	}

	Node* Parser::term() {
		Node* l = factor();
		
		do {
			Node* r = factor();
			if (r == nullptr) break;
			l = new CatNode(l, r);
		} while (1);
		return l;
	}
	Node* Parser::expr() {
		auto l = term();

		for (bool done = false; !done;)
		{
			auto t = _scanner.peek();
			l = std::visit(
				overload{
				  [=](C<'|'>) -> Node* { _scanner.drop(); return new AltNode(l, term()); },
				  [&](auto) -> Node* { done = true; return l; }
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
		Node* r1 = expr();
		r1 = std::visit(
			overload{
			  [=](C<'$'>) -> Node* { 
						_scanner.drop(); return new CatNode(new CatNode(r1, new MarkNode(rule_number, 1)), new CClassNode(make_cclass('\n'))); },
			  [=](C<'/'>) -> Node* { 
						_scanner.drop();
						Node* r2 = expr();
						return new CatNode(new CatNode(r1, new MarkNode(rule_number, 1)), r2); },
			  [&](auto) -> Node* { return new CatNode(r1, new MarkNode(rule_number, 1)); }
			}, t);
		r1 = new CatNode(r1, new MarkNode(rule_number, 0));
		if (!_scanner.match<eof>()) throw std::exception("Not finished parsing?");
		return Rule{ invert, r1 };
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
}
