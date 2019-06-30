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
	class CClass {
		cclass_t _cc;
	public:
		cclass_t& raw() { return _cc; }
		constexpr CClass() : _cc() {}
		constexpr CClass(cclass_t cc) : _cc(cc) {}
		constexpr bool empty() const { for (const auto& b : _cc) if (b != static_cast<size_t>(0)) false; return true; }
		void add(char c) { _private::_set_bit(_cc, (size_t)c); }
		void add_range(uint8_t s, uint8_t e) { for (uint8_t i = s; i <= e; i++) add(i); }
		bool at(size_t pos) const { return _private::_bit_is_set(_cc,pos); }
		size_t size() const {
			size_t count = 0;
			for (auto b : _cc)
				count += count_bits(b);
			return count;
		}
		bool operator[](size_t pos) const { return at(pos); }
		void flip() { for (auto& b : _cc) b = ~b; }
		constexpr  cclass_t& data() { return _cc; }
		constexpr const cclass_t& data() const { return _cc; }
		CClass& operator &=(const CClass& r) { for (size_t i = 0; i < _cc.size(); i++) _cc[i] &= r._cc[i]; return *this; }
		CClass& operator |=(const CClass& r) { for (size_t i = 0; i < _cc.size(); i++) _cc[i] |= r._cc[i]; return *this; }
	};
	static inline constexpr CClass operator~(const CClass& r) { return CClass({ ~r.data()[0],~r.data()[1], ~r.data()[2], ~r.data()[3] }); }
	static inline constexpr CClass operator&(const CClass& l, const CClass& r) { 
		return CClass({ l.data()[0] & r.data()[0], l.data()[1] & r.data()[1], l.data()[2] & r.data()[2],  l.data()[3] & r.data()[3] });
	}
	static inline constexpr CClass operator|(const CClass& l, const CClass& r) {
		return CClass({ l.data()[0] | r.data()[0], l.data()[1] | r.data()[1], l.data()[2] | r.data()[2],  l.data()[3] | r.data()[3] });
	}
	// because I keep changing CCClass, making these simple wrappers to help


	static constexpr CClass letters = make_cclass(crange('A', 'Z'), crange('a', 'z'), '_');
	static constexpr CClass digits = make_cclass(crange('0', '9'));
	static constexpr CClass digit_letters = make_cclass(crange('A', 'Z'), crange('a', 'z'), '_', crange('0', '9'));
	static constexpr CClass all_chars = make_cclass(crange(0, 255));
	static constexpr CClass dot_chars = make_cclass(crange(0, '\n' - 1), crange('\n' + 1, 255));

	static inline CClass operator-(const CClass& l, const CClass& r) {
		CClass cc = l;
		cc &= ~r;
		return cc;
	}
#if 0
	letters: CClass = ['A'..'Z', 'a'..'z', '_'];
		 digits: CClass = ['0'..'9'];
		 alphanums: CClass = ['A'..'Z', 'a'..'z', '_', '0'..'9'];
			 using IntSet = std::set<int>; // sorted int set
#endif
			 using InitSet = std::set<int>;
			 using InitSetPtr = std::shared_ptr<InitSet>;
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
			 template<size_t L, size_t R>
			 static constexpr inline bool inrange(size_t v) { return (L >= v) && (R <= v); }

			 template<typename T>
			 static inline void unescape_stream(std::ostream&  os, T c) {
				 if (inrange<0, 7>(c) || inrange<14, 31>(c) || inrange<127, 255>(c) || c == 11) {
					 os << '\\' << (char)((c / 64)) << (char)(((c % 64) / 8)) << (char)( (c % 8));
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
						 os << (char)c;
					 }
				 }

			 }
			 template<typename T, size_t N>
			 static inline void unescape_stream(std::ostream&  os, T c, const char(&reserved)[N]) {
				 for (char r : reserved)
					 if (c == r) {
						 os << '\\' << (char)c;
					 }
				 unescape_stream(os, c);
			 }


			// class node;
			 struct mark_node { int rule; int  pos; };
			 struct char_node { char c; };
			 struct string_node { std::string str; };
			 struct cclass_node { CClass cc; };
			 struct cat_node;
			 struct alt_node;
			 struct plus_node;
			 struct star_node;
			 struct opt_node;
			 using node = std::variant< mark_node, char_node, string_node, cclass_node, cat_node, alt_node, plus_node, star_node, opt_node>;
			 using nodep = std::unique_ptr<node>;
			 struct cat_node { nodep r1; nodep r2; };
			 struct alt_node { nodep r1; nodep r2; };
			 struct plus_node { nodep r; };
			 struct star_node { nodep r; };
			 struct opt_node { nodep r; };
			 static constexpr int NCH = 10000;
			 static constexpr int RCCL = NCH + 90;
			 static constexpr int  RNCCL = NCH + 91;
			 static constexpr int  RSTR = NCH + 92;
			 static constexpr int  RSCON = NCH + 93;
			 static constexpr int  RNEWE = NCH + 94;
			 static constexpr int FINAL = NCH + 95;
			 static constexpr int  RNULLS = NCH + 96;
			 static constexpr int  RCAT = NCH + 97;
			 static constexpr int  STAR = NCH + 98;
			 static constexpr int  PLUS = NCH + 99;
			 static constexpr int  QUEST = NCH + 100;
			 static constexpr int  DIV = NCH + 101;
			 static constexpr int  BAR = NCH + 102;
			 static constexpr int  CARAT = NCH + 103;
			 static constexpr int  S1FINAL = NCH + 104;
			 static constexpr int  S2FINAL = NCH + 105;
			 struct mn {

				 int name;
				 int left;
				 int right;
				 int parent;
				 bool nullstr;
				 mn(int a, int d, int c);
				 mn(int a, int d);
				 mn(int a);
			 };

			 std::ostream& operator<<(std::ostream& os, const node& n);
			 inline std::ostream& operator<<(std::ostream& os, const nodep& n) { os << *n; return os; }

			 struct micro_sym{ std::string subst; };
			 struct start_state_sym { int start_state; };
		
			 struct PosTableEntry {
				 InitSet follow_pos;
				 std::variant<char_node, cclass_node, mark_node> pos;
				 PosTableEntry(char_node&& c) : pos(std::move(c)) {}
				 PosTableEntry(cclass_node&& c) : pos(std::move(c)) {}
				 PosTableEntry(mark_node&& c) : pos(std::move(c)) {}
			 };
			 struct StateTableEntry {
				 int* state_pos;
				 bool final;
				 int trans_lo, trans_hi;
			 };
			 
			 struct TransTableEntry {
				 CClass* CClassPtr;
				 int* follow_pos;
				 int next_state;
			};
			 extern std::vector< PosTableEntry> pos_table;
			 extern std::vector<StateTableEntry> state_table;
			 extern std::vector<TransTableEntry> trans_table;
			 extern std::unordered_map<Symbol, std::variant< micro_sym, start_state_sym>> sym_table;
			 static inline void addCharPos(char c) {
				 pos_table.emplace_back(char_node { c } );
			 }
			 static inline  void addCClassPos(CClass cc) {
				 pos_table.emplace_back(cclass_node { cc } );
			 }
			 static inline void addMarkPos(int rule, int pos) {
				 pos_table.emplace_back( mark_node{ rule, pos });
			 }
#if 0
			 class node : public std::variant<eps_node, mark_node, char_node, string_node, cclass_node, cat_node, alt_node, plus_node, star_node, opt_node>
			 {
				 using base = std::variant<eps_node, mark_node, char_node, string_node, cclass_node, cat_node, alt_node, plus_node, star_node, opt_node>;
				 using base::base;
				 using base::operator=;
			 };
#endif
			

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
				 size_t n_rules;
				 std::vector<StateTableEntry> state_table;
				 std::vector<TransTableEntry> trans_table;
				 std::unordered_map<Symbol, std::variant< micro_sym, start_state_sym>> sym_table;
			 public:

				 nodep prec(nodep& r);
				 nodep expr();
				 nodep term();
				 nodep factor();
				 struct Rule {
					 bool invert;
					 nodep nodes;
				 };
				 Rule parse_rule(int rule_number, std::string_view rule);
				 void next_section() {}
				 void define_start_state(std::string_view name,int  pos) {
					 Symbol sym = name;
					 if (sym_table.emplace(std::make_pair(name, start_state_sym{ pos })).second) throw "state already exists";

				 }

				 void add_rule(int i, InitSet& FIRST) {

				 }
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