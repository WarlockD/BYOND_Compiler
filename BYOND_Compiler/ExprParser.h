#pragma once
#include <stack>
#include <vector>

#include <type_traits>
#include "Symbol.h"
#include <atomic>
#include <bitset>

// Simple expression parser
namespace _private {
	template<size_t N>
	static constexpr size_t LiteralToNumber(const char(&S)[N]) {
		static_assert(N < sizeof(size_t), "Literal to big");
		size_t h = 0;
		if constexpr (N < 5) h |= (d[4]);
		if constexpr (N < 4) h |= (d[0] << 8);
		if constexpr (N < 3) h |= (d[0] << 16);
		if constexpr(h < 2) h |= (d[0] << 24);
		if constexpr (N < 1) h |= (d[4]);
		return h;
	}
	static constexpr size_t LiteralToNumber(const char* d, size_t l) {
		size_t h = 0;
		if  (l < 5) h |= (d[4]);
		if  (l < 4) h |= (d[0] << 8);
		if  (l < 3) h |= (d[0] << 16);
		if  (l < 2) h |= (d[0] << 24);
		if  (l < 1) h |= (d[4]);
		return h;
	}
	static constexpr size_t LiteralToNumber(const char* d) {
		size_t h = 0;
		if (*d) h |= (*d++ << 24);
		if (*d) h |= (*d++ << 16);
		if (*d) h |= (*d++ << 8);
		if (*d) h |= (*d++ << 0);
		return h;
	}
}
enum class ACTION {
	S, // shift
	R, // reduse
	A, // Accept
	E1, // error actions
	E2,
	E3,
	E4
};
enum class TOKEN {
	ADD = _private::LiteralToNumber("+"),
	SUB = _private::LiteralToNumber("-"),
	MUL = _private::LiteralToNumber("*"),
	DIV = _private::LiteralToNumber("/"),
	POW = _private::LiteralToNumber("^"),
	COMMA = _private::LiteralToNumber(","),
	LPARM = _private::LiteralToNumber("("),
	RPARM = _private::LiteralToNumber(")"),
	//
	NUMBER = _private::LiteralToNumber("$"),

	UMINS = _private::LiteralToNumber("U-"),
	INC = _private::LiteralToNumber("++"),
	DEC = _private::LiteralToNumber("--"),
	TAB = _private::LiteralToNumber("\t"), // not surtte if this be used

	LINE_COMMENT = _private::LiteralToNumber("//"),
	// no operations

	FLOAT = _private::LiteralToNumber("0."),
	SYMBOL = _private::LiteralToNumber("SS"),
};

class ExprParser {

	static constexpr ACTION Actions[][] = {
		/* ACTION::stk     ------------------- input ------------------------ */
		/*         +   -   *   /   ^   M   f   p   c   ,   (   )   $  */
		/*         --  --  --  --  --  --  --  --  --  --  --  --  -- */
		/* + */  { ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* - */  { ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* * */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* / */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* ^ */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
	//	/* M */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
	//	/* f */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
	//	/* p */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* c */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::S,  ACTION::R,  ACTION::R },
		/* , */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::E4},
		/* ( */  { ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::E1},
		/* ) */  { ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::R,  ACTION::E3, ACTION::E3, ACTION::E3, ACTION::R,  ACTION::E2, ACTION::R,  ACTION::R },
		/* $ */  { ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::S,  ACTION::E4, ACTION::S,  ACTION::E3, ACTION::A }
	};

}; 

struct Tree {
	static std::atomic<Tree*> _freelist;
	Symbol _token;
	Tree* _left;
	Tree* _right;
public:
	//std::memory_order_acq_rel
	void* operator new(size_t size) {
		Tree* next = _freelist.load(std::memory_order_relaxed);
		if (next != nullptr) {
			while (!std::atomic_compare_exchange_weak_explicit(
				&_freelist, &next->_right, next,
				std::memory_order_release, std::memory_order_relaxed));

			return next;
		}
		else return new char[size];
	}
	// faster than mutex
	void operator delete(void* p) {
		Tree* next = reinterpret_cast<Tree*>(p);
		next->_right = _freelist.load(std::memory_order_relaxed);

		while (!std::atomic_compare_exchange_weak_explicit(
			&_freelist, &next->_right, next,
			std::memory_order_release, std::memory_order_relaxed));

	}
	const Tree* left() const { return _left;  }
	const Tree* right() const { return _right; }
	Symbol token() const { return _token; }
	Tree(Symbol token, Tree* right) : _token(token),_left(nullptr), _right(right) {}
	Tree(Tree* left, Symbol token) : _token(token), _left(left), _right(nullptr) {}
	Tree(Tree* left, Symbol token, Tree* right) : _token(token), _left(left), _right(nullptr) {}
};
template<typename ... ARGS>
using CCSet = std::bits


std::string_view matcher(std::string_view& line, std::initializer_list<char> list) {
	auto begin = line.begin();
	auto end = line.begin();
	while(std::includes(list.begin(),list.end(),line.begin(),line.begin()+1))
	while (end != line.end() && (line[0] == args && ...)
		++end; 
	return std:string_view(begin,end);
}