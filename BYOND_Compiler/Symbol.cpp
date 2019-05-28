#include "Symbol.h"

#include <assert.h>
#include <xatomic.h>
#include <mutex>
#include <vector>
#include <type_traits>




#if 0

const Symbol::str_ref* Symbol::_empty = nullptr;



// singleton only!
using hasher_t = std::hash<std::string_view>;
static hasher_t SymbolHasher;

/*
** 'module' operation for hashing (size is always a power of 2)
*/
static constexpr size_t LUAI_HASHLIMIT = sizeof(size_t) + 1;


template<typename T> inline static size_t lmod(T s, T size) { assert((size & (size - 1)) == 0); return s & (size - 1); }
template<typename T> inline static size_t twoto(T x) { return  (1 << (x)); }

//#define twoto(x)    
//#define sizenode(t)     (twoto((t)->lsizenode))

class SymbolTable {
	using str_ref = Symbol::str_ref;
	size_t _hash_seed;
	size_t _nused;
	std::vector<str_ref*>  _strt;

	str_ref* _newstring(const std::string_view& ss, size_t h) {
		size_t total_size = sizeof(str_ref) + ss.size() + 1; // include zero for string
		total_size = (total_size + sizeof(uintptr_t)) & sizeof(uintptr_t); // allign it to pointers
		char* data = new char[total_size];
		char* str = data + sizeof(str_ref);
		ss.copy(str, ss.size());
		str[ss.size()] = 0;
		return new(data) str_ref{ std::string_view(str,ss.size()), h };
	}

public:
	str_ref* intern(const std::string_view& ss) {
		size_t h = SymbolHasher(ss);
		size_t pos = lmod(h, _strt.size());
		for (str_ref* ts = _strt[pos]; ts != nullptr; ts = ts->_next) {
			if (ss.data() == ts->_data.data() || ss == ts->_data) return ts;
		}
		if (_nused >= _strt.size() && _strt.size() <= (std::numeric_limits<int>::max() / 2UL)) {
			g_resize_table(_strt, _strt.size() * 2);
		}
		str_ref* nts = _newstring(ss, h);
		nts->_next = _strt[pos];
		_strt[pos] = nts;
		_nused++;
		return nts;
	}
	SymbolTable() {
		_nused = 0;
		Symbol::_empty = intern(std::string_view(""));
	}

};
SymbolTable _global_symbols;



Symbol::Symbol(const std::string_view& ss) : _str(_global_symbols.intern(ss)) {}

class SymbolListTable {
	using list_ref = SymbolList::list_ref;
	size_t _hash_seed;
	size_t _nused;
	std::vector<list_ref*>  _strt;


}; 

#endif
