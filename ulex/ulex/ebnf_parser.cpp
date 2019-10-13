#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <exception>
#include <memory>
#include <fstream>
#include <set>
#include <bitset>
#include <optional>
#include <queue>

// simple string intern library
#include "../../btyaccpp/mstring.h"

#include <string_view>
#include <array>
#include <variant>

class Pos {
	int _line;
	int _colum;
public:
	Pos() : _line(-1), _colum(-1) {}
	Pos(int line, int column) : _line(line), _colum(column) {}
	void error(const std::string& msg) {
		std::stringstream ss;
		ss << "near line " << _line + ':' << msg;
		throw std::exception(ss.str().c_str());
	}
	int line() const { return _line; }
	int colum() const { return _colum; }
};
static constexpr size_t NCH = (sizeof(char) * 8 + 1);

enum class token_t : size_t {
	CHAR, CCL, NCCL, STR, DELIM, SCON, ITER, NEWE, NULLS
};
 enum class Kind : size_t {
	
	CHAR,
	RCCL,
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
};

 using cclass_t = std::bitset<256>;

class Node : public Pos {
public:
	using handle_t = int;
	static constexpr auto nullhandle = handle_t(-1);
public:

	static auto& nodes() { static std::vector<Node> nodes;  return nodes; }
	inline static Node* lookup_pointer(handle_t h) { return h == nullhandle ? nullptr : &nodes()[h]; }
	inline static handle_t lookup_handle(const Node* p) { return p == nullptr ? nullhandle : static_cast<handle_t>((p - nodes().data())/sizeof(Node)); }
	struct tree_t { handle_t left; handle_t right; };
	struct leaf_t { handle_t next; };

	struct cclass_tt { handle_t next; cclass_t str; };
	struct cchar_t { handle_t next; int c; };
	using value_t = std::variant<std::nullptr_t, leaf_t, tree_t, cclass_tt, cchar_t, string_t>;
	int _kind;
	value_t _value;
	bool _nullable;
	handle_t _parent; 
	inline Node(Pos pos, Kind kind, value_t value, bool is_nullable, const Node* p = nullptr) : Pos(pos), _kind(static_cast<int>(kind)), _value( value), _nullable(is_nullable), _parent(lookup_handle(p)) {}
	
	inline Node(Pos pos, int c, const  Node* p = nullptr) : Node(pos, static_cast<Kind>(c), value_t(nullptr),false,p) { }
	inline Node(Pos pos, Node* p = nullptr) : Node(pos, Kind::RNULLS, nullptr, true, p) { }

	inline Node(Pos pos, Kind kind, string_t str, Node* p = nullptr) : Node(pos, kind,value_t(str), !str ? true : false, p) { } // RCCL || RNCCL
	inline Node(Pos pos, Kind kind, Node* n, Node* p = nullptr) : Node(pos, kind, value_t(leaf_t{ lookup_handle(n) } ), kind == Kind::STAR || kind == Kind::QUEST || kind == Kind::S2FINAL ? true : n->_nullable, p) { } // STAR, QUEST, PLUS, CARAT, S2FINAL
	inline Node(Pos pos, Kind kind,  Node* r,  Node* l,  Node* p = nullptr) 
		: Pos(pos), _kind(static_cast<int>(kind)), _value(tree_t{ lookup_handle(l), lookup_handle(r) } ), _nullable(false), _parent(lookup_handle(p))
	{
		switch (kind) {
			case Kind::RSTR:
				l->_parent = lookup_handle(this);
				break;
			case Kind::BAR:
			case Kind::RNEWE:
				if (l->_nullable || r->_nullable) _nullable = true;
				l->_parent = r->_parent = lookup_handle(this);
				break;
			case Kind::RCAT:
			case Kind::DIV:
				if (l->_nullable && r->_nullable) _nullable = true;
				l->_parent = r->_parent = lookup_handle(this);
				break;
			case Kind::RSCON:
				l->_parent = lookup_handle(this);
				_nullable = l->_nullable;
				break;
		}
	} // STAR, QUEST, PLUS, CARAT, S2FINAL

public:
	using pointer = Node *;
	using const_pointer = const pointer;
	using refrence = Node &;
	using const_refrence = const pointer;
	const_pointer left() const { return lookup_pointer(std::get<2>(_value).left); }
	const_pointer right() const { return lookup_pointer(std::get<2>(_value).right); }
	const_pointer next() const { return lookup_pointer(std::get<1>(_value).next); }
	const_pointer parent() const { return lookup_pointer(_parent); }
	Kind type() const { return _kind < NCH ? Kind::CHAR : static_cast<Kind>(_kind); }
	operator Kind() const { return type(); }
	bool nullable() const { return _nullable;  }
	static Node* newChar(int c) {
		return nullptr;
	}

};






class Token {
	string_t _tag;
public:
	Token(string_t tag) : _tag(tag) {}
	string_t tag() const { return _tag; }

};
static inline  std::ostream&  operator<<(std::ostream& os, const Token& token) {
	os << token.tag();
	return os;
}
class Word : public Token {
	string_t _lexeme;
public:
	Word(string_t lexeme, string_t tag) : _lexeme(lexeme), Token(tag) {}
	string_t lexeme() const { return _lexeme; }
};

static inline  std::ostream& operator<<(std::ostream& os, const Word& token) {
	os << "Word " << token.lexeme() << ' '<< token.tag();
	return os;
}

class NonTerminal {
	string_t _symbol;
	std::set<int> _productions;
	std::vector<int> _first;
	std::vector<int> _follows;
	bool _nullable;
	NonTerminal(string_t symbol) : _symbol(symbol), _nullable(false) {}
};
// Stupid simple has program, can't remove
template<typename T>
class OrderedHash {
	std::unordered_map<string_t, int> _lookup;
	std::vector<T> _list;
public:
	using value = T;
	using refrence = &T;
	using const_refrence = const refrence;
	std::optional<refrence> lookup(string key) {
		auto it = _lookup.find(key);
		if (it == _lookup.end()) return std::nullopt;
		else return _list[it->second];
	}
	std::optional<const_refrence> lookup(string key) const {
		auto it = _lookup.find(key);
		if (it == _lookup.end()) return std::nullopt;
		else return _list[it->second];
	}
	refrence at(size_t i) { return _list[i]; }
	const_refrence at(size_t i) const { return _list[i]; }
	refrence operator[](size_t i) { return _list[i]; }
	const_refrence operator[](size_t i) const { return _list[i]; }
	auto begin() { reutrn _list.begin(); }
	auto end() { reutrn _list.end(); }
	auto begin() const { reutrn _list.begin(); }
	auto end() const { reutrn _list.end(); }
};

class Lexer {
	string_t _filename;
	std::string _source;
	std::string::const_iterator _index;
	Pos _pos;
	int peek, prev, pres;
	std::queue<int> _pushptr;
	int gch() {
		prev = pres;
		c = pres = peek;
		if (_pushptr.empty())
			peek = *_index++;
		else{
			peek = _pushptr.back();
			_pushptr.pop();
	}


	}
	std::vector<cclass_t> _cclasses;
	size_t cclinter(cclass_t n, bool invert) {
		if (invert) n.flip();
		for (size_t i = 0; i < _cclasses.size(); i++)
			if (_cclasses[i] == n) return i;
		size_t ret = _cclasses.size();
		_cclasses.emplace_back(n);
		return ret;
	}
	std::unordered_map<string_t, Word> _words;
	template<typename T>
	int ctrans(T& ss) {
		int c, k;
		if ((c = *ss) != '\\')
			return(c);
		switch (c = ++ss)
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
				*ss++;
			}
			break;
		}
		return(c);
	}
	int usescape(int c){
		register char d;
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
public:
	Lexer(string_t filename) {
		std::ifstream in(filename.str().data(), std::ios::in | std::ios::binary);
		if (!in) throw(errno);
		_source.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
	}
		


};