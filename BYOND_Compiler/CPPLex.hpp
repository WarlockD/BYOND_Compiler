#pragma once
#include <string_view>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <utility>
#include <variant>
#include <type_traits>
#include <cassert>
#include <iterator>
#include <queue>
#include "Symbol.h"

#include <variant>
#include <string>
#include <optional>
#include <string_view>
#include <array>

namespace PascalLexer {

	using char_t = char;
	using uchar_t = std::make_unsigned_t<char_t>;

	using crange = std::pair<size_t, size_t>;
	using cclass_t = std::array<uint64_t, 4>;

	namespace _private {
		template<typename T, size_t COUNT>
		inline static constexpr std::array<T, COUNT>  or_array(std::array<T, COUNT> l, std::array<T, COUNT> r) {
			cclass_t  cc{};
			for (size_t i = 0; i < COUNT; i++) cc[i] = l[i] | r[i];
			return cc;
		}
		inline static constexpr void  _set_bit(cclass_t& cc, size_t bit) {
			constexpr size_t BITS_PER_WORD = sizeof(uint64_t) * 8;
			cc[(bit / BITS_PER_WORD)] |= static_cast<uint64_t>(1) << (bit % BITS_PER_WORD);
		}
		inline static constexpr void  _clear_bit(cclass_t& cc, size_t bit) {
			constexpr size_t BITS_PER_WORD = sizeof(uint64_t) * 8;
			cc[(bit / BITS_PER_WORD)] &= ~(static_cast<uint64_t>(1) << (bit % BITS_PER_WORD));
		}
		inline static constexpr bool  _bit_is_set(const cclass_t& cc, size_t bit) {
			constexpr size_t BITS_PER_WORD = sizeof(uint64_t) * 8;
			return (cc[(bit / BITS_PER_WORD)] & static_cast<uint64_t>(1) << (bit % BITS_PER_WORD)) != 0;
		}
		inline static constexpr cclass_t _set_bits(size_t bit) {
			constexpr size_t BITS_PER_WORD = sizeof(uint64_t) * 8;
			cclass_t  cc{};
			cc[(bit / BITS_PER_WORD)] |= static_cast<uint64_t>(1) << (bit % BITS_PER_WORD);
			return cc;
		}
		inline static constexpr cclass_t _set_bits(char c) {
			return _set_bits(static_cast<size_t>(c));
		}
		inline static constexpr cclass_t _set_bits(uint8_t c) {
			return _set_bits(static_cast<size_t>(c));
		}
		inline static constexpr cclass_t _set_bits() {
			return {};
		}
		inline static constexpr cclass_t _set_bits(crange bits) {
			constexpr size_t BITS_PER_WORD = sizeof(uint64_t) * 8;
			cclass_t  cc{};
			for (size_t i = bits.first; i <= bits.second; i++)
				cc[(i / BITS_PER_WORD)] |= static_cast<uint64_t>(1) << (i % BITS_PER_WORD);
			return cc;
		}
		template<typename F, typename ... MORE>
		inline static constexpr cclass_t  _set_bits(F v, MORE ... more) {
			return or_array(_set_bits(v), _set_bits(std::forward<MORE>(more)...));
		}
	};
	template<typename ... MORE>
	inline static constexpr  cclass_t make_cclass(MORE ... more) {
		return _private::_set_bits(std::forward<MORE>(more)...);
	}
	using CClass = cclass_t;
#if 0
	class CClass {
		static constexpr size_t BIT_COUNT = 256;
		using type = uint64_t;
		static constexpr size_t	BITS_PER_WORD = 8 * sizeof(type);
		static constexpr size_t	WORDS = 4; //  BIT_COUNT == 0 ? 0 : (BIT_COUNT - 1) / BITS_PER_WORD;
		static constexpr type CLEAR_WORD = static_cast<type>(0);
		static constexpr type SET_WORD = CLEAR_WORD ^ CLEAR_WORD;
		//using cclass_t = std::array<type, WORDS>;
		cclass_t _cc;

		// constexpr init
		static constexpr cclass_t __init_set(size_t pos) {
			return {
				(pos / BITS_PER_WORD) == 0 ? static_cast<type>(1) << (pos % BITS_PER_WORD) : 0,
				(pos / BITS_PER_WORD) == 1 ? static_cast<type>(1) << (pos % BITS_PER_WORD) : 0,
				(pos / BITS_PER_WORD) == 2 ? static_cast<type>(1) << (pos % BITS_PER_WORD) : 0,
				(pos / BITS_PER_WORD) == 3 ? static_cast<type>(1) << (pos % BITS_PER_WORD) : 0,
			};
		}
		static constexpr cclass_t __combine(cclass_t l, cclass_t r) {
			return { l[0] | r[0], l[1] | r[1], l[2] | r[2], l[3] | r[3] };
		}
		static constexpr cclass_t __init() { return { 0, 0, 0, 0, }; }
		static constexpr cclass_t __init(size_t pos) { return __init_set(pos); }
		static constexpr cclass_t __init(uint8_t pos) { return __init_set(pos); }
		static constexpr cclass_t __init(char pos) { return __init_set(pos); }
		static constexpr cclass_t __init(CClass pos) { return pos._cc; }
		// constexpr init
		template<typename L, typename R>
		static constexpr cclass_t __init(std::pair<L, R> pos) {
			cclass_t cc = { 0,0,0,0 };
			for (size_t i = pos.first; i <= pos.second; i++) cc = __combine(cc, __init(i));
			return cc;
		}

		template<typename T, typename ... MORE>
		static constexpr inline  cclass_t __init(T v, MORE ... more) { return __combine(__init(v), __init(std::forward<MORE>(more)...)); }
		template<typename ... MORE>
		static constexpr inline  cclass_t _init(MORE ... more) { return __init(std::forward<MORE>(more)...); }

		bool _get(size_t pos) const { return (_cc[pos / BITS_PER_WORD] & (static_cast<type>(1) << (pos % BITS_PER_WORD))) != 0; }
		inline void _set(size_t pos) { _cc[pos / BITS_PER_WORD] |= static_cast<type>(1) << (pos % BITS_PER_WORD); }
		inline void _set(uint8_t pos) { _set(static_cast<size_t>(pos)); }
		inline void _set(char pos) { _set(static_cast<size_t>(pos)); }
		inline void _clear(size_t pos) { _cc[pos / BITS_PER_WORD] &= ~(static_cast<type>(1) << (pos % BITS_PER_WORD)); }
		inline void _clear(uint8_t pos) { _clear(static_cast<size_t>(pos)); }
		inline void _clear(char pos) { _clear(static_cast<size_t>(pos)); }
		// recursive sets
		inline  void _set(crange range) { for (size_t c = range.first; c <= range.second; c++) _set(static_cast<size_t>(c)); }
		inline  void _clear(crange range) { for (size_t c = range.first; c <= range.second; c++) _clear(static_cast<size_t>(c)); }
		template<typename T, typename ... MORE>
		inline  void _set(T&& v, MORE ... more) { _set(v); _set(std::forward<MORE>(more)...); }
		template<typename T, typename ... MORE>
		inline  void _clear(T&& v, MORE ... more) { _clear(v); _clear(std::forward<MORE>(more)...); }
	public:
		constexpr CClass() : _cc() {}
		constexpr CClass(cclass_t cc) : _cc(cc) {}
		bool empty() const { for (const auto& b : _cc) if (b != static_cast<size_t>(0)) false; return true; }
		template<typename ... T> constexpr void set(T ... v) { _set(std::forward<T>(v)...); }
		template<typename ... T> constexpr void clear(T ... v) { _set(std::forward<T>(v)...); }
		bool at(size_t pos) const { return _get(pos); }
		bool operator[](size_t pos) const { return _get(pos); }
		void flip() { for (auto& b : _cc) b ^= CLEAR_WORD; }
	};
	// because I keep changing CCClass, making these simple wrappers to help
#endif
	static inline void add_char_to_cclass(CClass& cc, char c) { _private::_set_bit(cc, c); }
	static inline void add_range_to_cclass(CClass& cc, size_t f, size_t l) {
		for (size_t i = f; i <= l; i++)_private::_set_bit(cc, i);
	}
	static inline void flip_cclass(CClass& cc) {
		for (auto& c : cc)  c = ~c;
	}
	template<typename T>
	static inline bool char_in_class(const CClass& cc,T c) {
		return _private::_bit_is_set(cc, c);
	}
#define _USE_FAST
#ifdef _USE_FAST
#include <intrin.h>
#endif
	// https://codingforspeed.com/a-faster-approach-to-count-set-bits-in-a-32-bit-integer/
	// nice approce
	inline int bitCount2(uint32_t i) {
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		i = (i + (i >> 4)) & 0x0f0f0f0f;
		i = i + (i >> 8);
		i = i + (i >> 16);
		return i & 0x3f;
	}
	 static inline size_t count_bits(uint64_t bits) {
#ifdef _USE_FAST
		return __popcnt64(bits);
#else
		 return bitCount2(static_cast<uint32_t>(bits)) + bitCount2(static_cast<uint32_t>(bits >> 32));
		static constexpr uint8_t num_to_bits[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
		if (bits == 0) return num_to_bits[0];

#endif
	}
	 static inline size_t nchars(const CClass& cc) {
		 size_t count = 0; 
		 for (uint64_t b : cc) 
			 count += count_bits(b); 
		 return count;
	}
	static constexpr CClass letters = make_cclass(crange('A', 'Z'), crange('a', 'z'), '_');
	static constexpr CClass digits = make_cclass(crange('0', '9'));
	static constexpr CClass digit_letters = make_cclass(crange('A', 'Z'), crange('a', 'z'), '_', crange('0', '9'));
	static constexpr CClass all_chars = make_cclass(crange(0, 255));
	static constexpr CClass dot_chars = make_cclass(crange(0, '\n'-1), crange('\n' + 1,255));

	static inline cclass_t operator-(const cclass_t& l, const cclass_t& r) {
		cclass_t cc;
		std::set_difference(l.begin(), l.end(), r.begin(), r.end(), cc.begin());
		return cc;
	}
#if 0
	letters: CClass = ['A'..'Z', 'a'..'z', '_'];
		 digits: CClass = ['0'..'9'];
		 alphanums: CClass = ['A'..'Z', 'a'..'z', '_', '0'..'9'];
			 using IntSet = std::set<int>; // sorted int set
#endif
			 using InitSet = std::set<int>;

			 static inline void empty(InitSet& M) { M.clear(); }
			 static inline void include(InitSet& M, int i) { M.insert(i); }
			 static inline void exclude(InitSet& M, int i) { M.erase(i); }
			 static inline void setunion(InitSet& M, const InitSet& N) {
				 InitSet K;
				 std::set_union(M.begin(), M.end(), N.begin(), N.end(), std::insert_iterator(K, K.end()));
				 M = std::move(K);
			 }
			 static inline void setminus(InitSet& M, const InitSet& N) {
				 InitSet K;
				 std::set_difference(M.begin(), M.end(), N.begin(), N.end(), std::insert_iterator(K, K.end()));
				 M = std::move(K);
			 }
			 static inline void intersect(InitSet& M, const InitSet& N) {
				 InitSet K;
				 std::set_intersection(M.begin(), M.end(), N.begin(), N.end(), std::insert_iterator(K, K.end()));
				 M = std::move(K);
			 }
			 static inline size_t size(const InitSet& M) { return M.size(); }
			 static inline bool member(int i, const InitSet& M) { return M.find(i) != M.end(); }
			 static inline bool isempty(const InitSet& M) { return M.empty(); }
			 static inline bool equal(const InitSet& M, const InitSet& N) { return M == N; }
			 static inline bool subseteq(const InitSet& M, const InitSet& N) { return std::includes(M.begin(), M.end(), N.begin(), N.end()); }

			 enum class NodeType {
				 mark,     // (* marker node *)
				 ch,			//  char_node, (*character node *)
				 str, // (*string node *)
				 cclass, // (*character class node *)
				 star, // (*star node *)
				 plus, //  (*plus node *)
				 opt, // (*option node *)
				 cat, // (*concatenation node *)
				 alt //  (*alternatives node(| ) *)
			 };
			 constexpr const char* NodeTypeString[] = {
				 "mark",
				 "ch",
				 "str",
				 "cclass",
				 "star",
				 "plus",
				 "opt",
				 "cat",
				 "alt"
			 };
			 struct Node {
				 const NodeType node_type;
			 protected:
				 Node(NodeType type) : node_type(type) {}
			 };

			 struct MarkNode : public Node {
				 int rule;
				 int pos;
				 MarkNode(int rule, int pos) : Node(NodeType::mark), rule(rule), pos(pos) {}
			 };
			 static inline std::ostream& operator<<(std::ostream&  os, const MarkNode& n) {
				 os << "#( rule: " << n.rule << " pos: " << n.pos << " ) ";
				 return os;
			 }
			 struct CharNode : public Node {
				 char c;
				 CharNode(char c) : Node(NodeType::ch), c(c) {}
			 };
			 template<size_t L, size_t R>
			 static constexpr inline bool inrange(size_t v) { return L >= v && R <= v; }

			 template<typename T>
			 static inline void unescape_stream(std::ostream&  os, T c) {
				 if (inrange<0, 7>(c) || inrange<14, 31>(c) || inrange<127, 255>(c) || c == 11) {
					 os << '\\' << (char)('0' + (c / 64)) << (char)('0' + ((c % 64) / 8)) << (char)('0' + (c % 8));
				 } // not printable
				 else {
					 switch (c) {
					 case '\b': os << "\\b"; break;
					 case '\t': os << "\\t"; break;
					 case '\n': os << "\\n"; break;
					 case '\r': os << "\\r"; break;
					 case '\f': os << "\\f"; break;
					 case '\\': os << "\\\\"; break;
					 default:
						 os << c;
					 }
				 }

			 }
			 template<typename T,size_t N>
			 static inline void unescape_stream(std::ostream&  os, T c, const char(&reserved)[N]) {
				 for(char r : reserved) 
					 if (c == r) {
						 os << '\\' << c;
					 }
				 unescape_stream(os, c);
			 }
		
			 static inline std::ostream& operator<<(std::ostream&  os, const CharNode& n) {
				 os  << " c: '";
				 unescape_stream(os, n.c, "\".^$[]*+?{}|(,)/<>]");
				 os << '\'';
				 return os;
			 }
			 struct StringNode : public Node {
				 std::string str;
				 StringNode(std::string&& cc) : Node(NodeType::str), str(std::move(str)) {}
				 StringNode(const std::string& cc) : Node(NodeType::str), str(str) {}
			 };
			 static inline std::ostream& operator<<(std::ostream&  os, const StringNode& n) {
				 os  << " str: \"";
				 for(char c : n.str)
					unescape_stream(os, c);
				 os << '\"';
				 return os;
			 }
			 struct CClassNode : public Node {
				 CClass cc;
				 CClassNode(const CClass& cc) : Node(NodeType::cclass), cc(cc) {}
				 CClassNode(CClass& cc) : Node(NodeType::cclass), cc(cc) {}
			 };
			 static inline std::ostream& operator<<(std::ostream&  os, const CClassNode& n) {
				 os << " cc: [";
				 size_t nc = nchars(n.cc);
				 if (nc == 254 && !char_in_class(n.cc, '\n'))
					 os << '.';
				 else {
					 cclass_t cc = n.cc;
					 if (nc > 128) { // more in likly inverted class
						 os << '^';
						 cc = all_chars - cc;
					 }
					 char c1 = 0;
					 bool done = false;
					 for (size_t c1 = 0;  c1< std::numeric_limits<uint8_t>::max(); c1++) {
						 // faster way to do this is to go over each bit one after another
						 if (char_in_class(cc, c1)) {
							 size_t c2 = c1;
							 while (c2 <= std::numeric_limits<uint8_t>::max() && char_in_class(cc, c2 + 1)) c2++;
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


			 struct CatNode : public Node {
				 Node* r1;
				 Node* r2;
				 CatNode(Node* r1, Node* r2) : Node(NodeType::cat), r1(r1), r2(r2) {}
				 ~CatNode() { if (r1) delete r1; if (r2) delete r2; }
			 };

			 struct AltNode : public Node {
				 Node* r1;
				 Node* r2;
				 AltNode(Node* r1, Node* r2) : Node(NodeType::alt), r1(r1), r2(r2) {}
				 ~AltNode() { if (r1) delete r1; if (r2) delete r2; }
			 };

			 struct PlusNode : public Node {
				 Node* r;
				 PlusNode(Node* r) : Node(NodeType::plus), r(r) {}
				 ~PlusNode() { if (r) delete r; }
			 };

			 struct StarNode : public Node {
				 Node* r;
				 StarNode(Node* r) : Node(NodeType::star), r(r) {}
				 ~StarNode() { if (r) delete r; }
			 };

			 struct OptNode : public Node {
				 Node* r;
				 OptNode(Node* r) : Node(NodeType::opt), r(r) {}
				 ~OptNode() { if (r) delete r; }
			 };


			 static inline std::ostream& operator<<(std::ostream&  os, const Node& n) {
				 switch(n.node_type) {
				 case NodeType::mark: 	 os << static_cast<const MarkNode&>(n); break;
				 case NodeType::ch: 	 os << static_cast<const CharNode&>(n); break;
				 case NodeType::str: 	 os << static_cast<const StringNode&>(n); break;
				 case NodeType::cclass: 	 os << static_cast<const CClassNode &> (n); break;
				 case NodeType::star: 	 os << static_cast<const StarNode&>(n); break;
				 case NodeType::plus: 	 os << static_cast<const PlusNode&>(n); break;
				 case NodeType::opt: 	 os << static_cast<const OptNode&>(n); break;
				 case NodeType::cat: 	 os << static_cast<const CatNode&>(n); break;
				 case NodeType::alt: 	 os << static_cast<const AltNode&>(n); break;
				 }
				 os << " node_type: " << NodeTypeString[(int)n.node_type];
				 return os;
			 }

			 static inline std::ostream& operator<<(std::ostream&  os, const CatNode& n) {
				 os << "( " << *n.r1 << *n.r2 << ")";
				 return os;
			 }
			 static inline std::ostream& operator<<(std::ostream&  os, const AltNode& n) {
				 os << *n.r1 << " | " << *n.r2;
				 return os;
			 }
			 static inline std::ostream& operator<<(std::ostream&  os, const StarNode& n) {
				 os << *n.r << '*';
				 return os;
			 }
			 static inline std::ostream& operator<<(std::ostream&  os, const PlusNode& n) {
				 os << *n.r << '+';
				 return os;
			 }
			 static inline std::ostream& operator<<(std::ostream&  os, const OptNode& n) {
				 os << *n.r << '?';
				 return os;
			 }
			 template <char CC>
			 struct C { static constexpr char c = CC; };

			 struct number { int value; };
			 struct eof {};
			 struct remember {};
			 struct forget {};
			 using token = std::variant <
				 number,
				 char,
				 CClass,
				 std::string,
				 C<'^'>,
				 C<'-'>,
				 C<'+'>,
				 C<'/'>,
				 C<'*'>,
				 C<'('>,
				 C<')'>,
				 C<'{'>,
				 C<'}'>,
				 C<','>,
				 C<'|'>,
				 C<'.'>,
				 C<'?'>,
				 C<'='>,
				 C<'$'>,
				 eof,
				 remember,
				 forget
			 > ;

			 constexpr  static CClass space_class = make_cclass(' ', '\t', '\f', '\r', '\n');
			 constexpr static CClass word_class = make_cclass(crange('A', 'Z'), crange('a', 'z'), crange('0', '9'));

			 class Scanner {
				 std::string_view _str;
				 std::string_view::iterator _pos;
				 std::optional<token> lookahead;
				 int scan_escape();
			 public:
				 //static inline constexpr bool _isalpha(int c) { return isalpha(c); }
				 Scanner() {}
				 using check_type = int(*)(int); // get the type of our coming is functions, or make custom? humm
				 using char_func = std::pair< int(*)(int), int(*)(int)>;
				 using char_peek = std::pair< char, char>;
				 Scanner(const std::string_view& v) : _str(v), _pos(_str.begin()), lookahead(std::nullopt) {}


				 token scan_token();
				 void drop();
				 token peek();
				 token next();
				 template<typename T>
				 bool match(T& v) {
					 if (std::holds_alternative<T>(peek())) {
						 // if constexpr (std::is_move_assignable_v<T>)
						 v = std::move(std::get<T>(peek()));
						 // else if constexpr(std::is_copy_assignable_v<T>)
						 //	 v = std::get<T>(peek());
						 drop();
						 return true;
					 }
					 return false;
				 }
				 template<typename T>
				 bool match() {
					 if (std::holds_alternative<T>(peek())) {
						 drop();
						 return true;
					 }
					 return false;
				 }
			 };


			 class Parser {
				 Scanner _scanner;
				 token _token;
			 public:
				 Parser() {}

				 Node* prec(Node* r);
				 Node* expr();
				 Node* term();
				 Node* factor();
				 struct Rule {
					 bool invert;
					 Node* nodes;
				 };
				 Rule parse_rule(int rule_number, std::string_view rule);
			 };

			 struct NFA_Table {
		
				 using int_t = size_t;
				 static constexpr int_t max_pos = 1200;  // maximum number of positions                 
				 static constexpr int_t max_states = 600;  //number of DFA states                        
				 static constexpr int_t max_trans = 1200;  //number of transitions                       
				 static constexpr int_t max_start_states = 100;  //maximum number of user - defined start states
				 int_t n_pos;
				 int_t n_states;
				 int_t n_trans;
				 int_t n_start_states;
				 struct none_sym {};
				 using macro_sym = std::string;
				 using start_state = int_t;
				 using sym_t = std::variant<none_sym, macro_sym, start_state>;
				 std::unordered_map<Symbol, sym_t> SymTable;
				 struct char_pos { char c; };
				 struct mark_pos { int_t rule, pos; };
				 struct cclass_pos { CClass* cc; };
				 struct PosSymbolEntry {
					 Symbol pname;
					 std::variant<none_sym, macro_sym, start_state> value;
				 };
				 struct PosTableEntry {
					 int_t* follow_pos;
					 std::variant<char, mark_pos, cclass_pos> value;
				 };
				 std::vector<PosTableEntry> PosTable;
				 std::array<int_t*, 2 * max_start_states + 1> IntSetPtr;
				 struct StateTableEntry {
					 int_t* state_pos;
					 bool final;
					 int_t trans_lo;
					 int_t* trans_hi;
				 };
				 std::array< StateTableEntry, max_states - 1> StateTableEntry;
				 struct TransTableEntry {
					 CClass* cc;
					 int_t* follow_pos;
					 int_t next_state;
				 };
				 std::array< TransTableEntry, max_trans + 1> TransTableEntry;

				 bool verbose;//          (*status of the verbose option *)
				 bool optimize;//         (*status of the optimization option *)
				 int_t act_state;

				 void addCharPos(char c);
				 void addCClassPos(CClass &cc);
				 void addMarkPos(int_t rule, int_t pos);
				 int_t newState(int_t* POS);
				 int_t addState(int_t* POS);
				 void startStateTrans();
				 void endStateTrans();
				 int_t n_state_trans(int_t i);
				 void addCClassPos(const CClass &cc, int_t* FOLLOW);
				 void mergeTrans();
				 void sortTrans();

			 };

};