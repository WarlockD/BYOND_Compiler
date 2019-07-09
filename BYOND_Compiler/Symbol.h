#pragma once

#include <ostream>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <random>
#include <string>
#include <sstream>
#include <fstream>
#include <variant>

// just because we got so many of these
template<typename T>
struct HashInterface {
	virtual size_t hash() const = 0;
	virtual int compare(const T& r) const = 0;
	virtual ~HashInterface() {}
};
namespace std {
	template<typename T>
	struct hash<HashInterface<T>> {
		size_t operator()(const HashInterface<T>& s) const { return s.hash(); }
	};
	template<typename T>
	struct equal_to<HashInterface<T>> {
		bool operator()(const HashInterface<T>& l, const HashInterface<T>& r) const { return l.compare(r) == 0; }
	};
}
namespace _private {
	// helper hash set class
	template<typename T, typename _HASHER>
	class TableContainer {
		// singleton only!

	public:
		using type = T;
		static constexpr size_t type_size = sizeof(type);
		using pointer = T * ;
		using const_pointer = const T*;
		using refrence = T & ;
		using const_refrence = const T&;
		using hasher_t = std::hash<T>;
		template<typename T> inline static size_t lmod(T s, T size) { assert((size & (size - 1)) == 0); return s & (size - 1); }
		template<typename T> inline static size_t twoto(T x) { return  (1 << (x)); }


		// defined in class so we can inline access.
		class ref_type {
			// we put string view in here so we can pass the const ref around
			// instead of recreating it all the time
			// I am sure the compiler would figure it out but eh.
			size_t _size;
			size_t _hash;
			ref_type* _next;
			friend class TableContainer<T, _HASHER>;
		public:
			ref_type(size_t s, size_t h) : _size(s), _hash(h), _next(nullptr) {}
#pragma warning(disable:4302)
			inline const_pointer data() const { return reinterpret_cast<const_pointer>(reinterpret_cast<const uint8_t>(this) + sizeof(ref_type)); }
			size_t size() const { return _size; }
			size_t hash() const { return _hash; }
		};

		static constexpr size_t  ref_size = sizeof(ref_type);
		using ref_pointer = ref_type * ;


		size_t _nused;
		ref_pointer _empty;
		std::vector<ref_pointer> _table;


		void _resize_table(size_t newsize) {
			// inspired from lua
			size_t old_size = _table.size();
			if (newsize > _table.size()) {
				_table.resize(newsize);
			}
			// rehash
			for (size_t i = 0; i < _table.size(); i++) {  /* rehash */
				auto p = _table[i];
				_table[i] = nullptr;
				while (p) {  /* for each node in the list */
					auto hnext = p->_next;  /* save next */
					size_t h = lmod(p->_hash, newsize);  /* new position */
					p->_next = _table[h];  /* chain it */
					_table[h] = p;
					p = hnext;
				}
			}
		}
		static inline constexpr size_t align_up(size_t num) { return  (num+sizeof(uintptr_t) -1) & ~(sizeof(uintptr_t) -1);  }
		// special case for string view
		ref_pointer _newref(const_pointer  d, size_t s, size_t h) {
			size_t total_size = ref_size + (type_size * s) ; // include zero for string
			if  (type_size == 1) total_size++; // an array? incrment it for the zero
			total_size = align_up(total_size); // allign it to pointers
			char* data = new char[total_size];
			pointer str = reinterpret_cast<pointer>(data + ref_size);
			std::copy_n(d, s, str);
			if  (type_size == 1) str[s] = type{};
			return new(data) ref_type(s, h);
		}

		ref_pointer intern(const_pointer d, size_t s) {
			static _HASHER _hasher;
			size_t h = _hasher(d,s);
			size_t pos = lmod(h, _table.size());
			for (ref_type* ts = _table[pos]; ts != nullptr; ts = ts->_next) {
				if ((d == ts->data() && s == ts->_size) ||
					( std::memcmp(d,ts->data(),s) == 0)) return ts;
			}
			if (_nused >= _table.size() && _table.size() <= (std::numeric_limits<int>::max() / 2UL)) {
				_resize_table(_table.size() * 2);
				pos = lmod(h, _table.size()); // get the new position
			}
			ref_type* nts = _newref(d,s, h);
			nts->_next = _table[pos];
			_table[pos] = nts;
			_nused++;
			return nts;
		}
		TableContainer() {
			_table.resize(32);
			_nused = 0;
			type b;
			_empty = intern(&b,0);
		}
	};




};
class Symbol : public HashInterface<Symbol>
{
	static size_t random_seed() {
		std::random_device rd;   // non-deterministic generator
		std::mt19937 gen(rd());  // to seed mersenne twister.
		return gen();
	}
	static constexpr size_t LUAI_HASHLIMIT = 5;
	static size_t luaS_hash(const char *str, size_t l) {
		static const size_t seed = random_seed(); 
		size_t h = seed ^  l;
		size_t step = (l >> LUAI_HASHLIMIT) + 1;
		for (; l >= step; l -= step)
			h ^= ((h << 5) + (h >> 2) + static_cast<uint8_t>(str[l - 1]));
		return h;
	}
	struct _hasher {
		size_t operator()(const char* d, size_t s) {
			if (s < 5) {
				size_t h = 0;
				if (h < 5) h |= ((size_t)d[4]);
				if (h < 4) h |= ((size_t)d[3] << 24);
				if (h < 3) h |= ((size_t)d[2] << 16);
				if (h < 2) h |= ((size_t)d[1] << 8);
				if (h < 1) h |= ((size_t)d[0]);
				return h;
			}
			else {
				return luaS_hash(d,s);
			}
		}
	};

	using table_type = _private::TableContainer<char, _hasher>;
	static table_type& singleton() {
		static table_type tbl;
		return tbl;
	}
	static table_type _strtbl;
	using ref_type = table_type::ref_type;
	const ref_type* _ref;
	Symbol(const ref_type* ref) : _ref(ref) {}
public:
	inline Symbol(const char* str, size_t len) : Symbol(singleton().intern(str, len)) {}
	inline Symbol(const std::string_view& sv) : Symbol(sv.data(),sv.size()) {}
	inline Symbol(const char* str) : Symbol(str,strlen(str)) {}
	inline Symbol(const std::string& ss) : Symbol(ss.data(), ss.length()) {}
	inline Symbol() : Symbol(_strtbl._empty) {}
	// wew have to atleast override moves because of _empty but lets do them all
	inline Symbol(const Symbol& copy) : Symbol(copy._ref) {}
	inline Symbol& operator=(const Symbol& copy) { _ref = copy._ref; return *this; }
	inline Symbol(Symbol&& move)noexcept : Symbol(move._ref) { move._ref = _strtbl._empty; }
	inline Symbol& operator=(Symbol&& move) noexcept { _ref = move._ref; move._ref = _strtbl._empty;  return *this; }

	// const char* c_str() const { return _str->str.data(); } // in theory its 0 terminated, but honestly this dosn't matter in symbols
	size_t hash() const { return _ref->hash(); }
	const char* data() const { return _ref->data(); }
	size_t size() const { return _ref->size(); }
	inline std::string_view str() const { return std::string_view(data(), size()); }
	inline operator const std::string_view&() const { return str(); }
	int compare(const Symbol& r) const { return _ref == r._ref ? 0 : str().compare(r.str()); }
	inline bool operator==(const Symbol& r) const { return _ref == r._ref; }
	inline bool operator!=(const Symbol& r) const { return _ref != r._ref; }
	void print(std::ostream& os) { os << str(); 	}
};
static inline std::ostream& operator<<(std::ostream& os, const Symbol& s) {
	os << s.str();
	return os;
}

namespace std {
	template<>
	struct hash<Symbol> {
		size_t operator()(const Symbol& s) const { return s.hash(); }
	};
	template<>
	struct equal_to<Symbol> {
		bool operator()(const Symbol& l, const Symbol& r) const { return l.compare(r) == 0; }
	};
}

template<typename T>
class array_view {
public:
	using type = T;
	using pointer = T * ;
	using const_pointer = const T*;
	using refrence = T & ;
	using const_refrence = const T&;
private:
	const_pointer _data;
	size_t _size;
public:
	array_view(const_pointer data, size_t s) : _data(data), _size(s) {}
	template<size_t S>
	array_view(T(&d)[S]) : array_view(d, S) {}
	size_t size() const { return _size; }
	const_pointer data() const { return _data; }
};
class token {
	int pos;
	Symbol name;
};
struct indent_t { int ident; };
struct string_t { Symbol str;  };
struct tok { int value;  };
using token = std::variant<tok, Symbol, indent_t, indent_t, int,float>;
struct line_t {
	int lineno;
	int ident;
	std::vector<token> tokens;
};

class file {
	std::string _filename;
	std::string data;
	std::vector< line_t> lines;
	enum class STATE {
		IDENT,
		STRING,
		COMMENT
	};
	size_t count_spaces(std::string& lilne, size_t& pos) {

	}
public:
	file(std::istream& input) : _lineno(0), _pos(0) {
		STATE state = STATE::IDENT;
		size_t current_line = 0;
		for (std::string line; std::getline(input, line); ) {
			size_t pos = 0;
			line_t line_tokens;
			line_tokens.lineno = ++current_line;
			line_tokens.ident = 0;
			for (char c : line) {
				switch (c) {
				case ' ':
				case '\t':
					switch (state) {

					}


				}
			}
			while(isspace(line[pos])
			sum += std::stoi(line);
		}

		std::string line;
		do {
			std::getline(fs, line);
			fs.tellg
		} while (true);
		while () {

		}
		std::getline()
	}
};
#if 0
class SymbolList : public HashInterface<SymbolList>
{
	struct _hasher {
		size_t operator()(const Symbol* d, size_t s) {
			size_t h = 0;
			for (size_t i = 0; i < s; i++)
				h ^= d->hash();
			return h;
		}
	};
	using table_type = _private::TableContainer<Symbol, _hasher>;
	static table_type& singleton() {
		static table_type tbl;
		return tbl;
	}
	using ref_type = table_type::ref_type;
	const ref_type* _ref;
	SymbolList(const ref_type* ref) : _ref(ref) {}
public:
	SymbolList(const Symbol* data, size_t s) : SymbolList(singleton().intern(data,s)) {}
	template<size_t S>
	SymbolList(const Symbol(&d)[S]) : SymbolList(d, S) {}
	SymbolList(const std::vector<Symbol>& data) : SymbolList(data.data(),data.size()) {}
	inline SymbolList() : SymbolList(singleton()._empty) {}
	// wew have to atleast override moves because of _empty but lets do them all
	inline SymbolList(const SymbolList& copy) : SymbolList(copy._ref) {}
	inline SymbolList& operator=(const SymbolList& copy) { _ref = copy._ref; return *this; }
	inline SymbolList(SymbolList&& move) noexcept  : SymbolList(move._ref)  { move._ref = singleton()._empty; }
	inline SymbolList& operator=(SymbolList&& move) noexcept { _ref = move._ref; move._ref = singleton()._empty;  return *this; }

	const Symbol* data() const { return _ref->data(); }
	Symbol operator[](size_t i) const { return data()[i]; }
	Symbol at(size_t i) const { return data()[i]; }
	const Symbol* begin() const { return data(); }
	const Symbol* end() const { return data()+size(); }
	// const char* c_str() const { return _str->str.data(); } // in theory its 0 terminated, but honestly this dosn't matter in symbols
	size_t hash() const { return _ref->hash(); }

	size_t size() const { return _ref->size(); }
	int compare(const SymbolList& r) const { return data() == r.data() ? 0: -1; }
	inline bool operator==(const SymbolList& r) const { return _ref == r._ref; }
	inline bool operator!=(const SymbolList& r) const { return _ref != r._ref; }
	void print(std::ostream& os, char delm = '/') {
		for (size_t i = 0; i < size(); i++) {
			os << delm;
			at(i).print(os);
		}
		
	}
};
#endif
#if 0

class Symbol : public HashInterface<Symbol>
{
	// defined in class so we can inline access.
	class str_ref  {
		// we put string view in here so we can pass the const ref around
		// instead of recreating it all the time
		// I am sure the compiler would figure it out but eh.
		std::string_view _data;
		size_t _hash;
		str_ref* _next;
		friend class SymbolTable;
	public:
		str_ref(std::string_view sv, size_t hash) : _data(sv), _hash(hash) {}
		const std::string_view& str() const { return _data; }
		size_t hash() const { return _hash;  }
	};
	static const str_ref* _empty;
	const str_ref* _str;
	 friend class SymbolTable;
	 Symbol(const str_ref* str) : _str(str) {}
public:
	Symbol(const std::string_view& sv);
	inline Symbol() : Symbol(_empty) {}
	inline Symbol(const char* str) : Symbol(std::string_view(str)) {}
	inline Symbol(const char* str, size_t len) : Symbol(std::string_view(str, len)) {}
	// wew have to atleast override moves because of _empty but lets do them all
	inline Symbol(const Symbol& copy) : Symbol(copy._str) {}
	inline Symbol& operator=(const Symbol& copy) { _str = copy._str; return *this; }
	inline Symbol(Symbol&& move) : Symbol(move._str) { move._str = _empty; }
	inline Symbol& operator=(Symbol&& move) { _str = move._str; move._str = _empty;  return *this; }

	// const char* c_str() const { return _str->str.data(); } // in theory its 0 terminated, but honestly this dosn't matter in symbols
	size_t hash() const { return _str->hash(); }
	inline const std::string_view& str() const { return _str->str(); }
	inline operator const std::string_view&() const { return str(); }
	int compare(const Symbol& r) const { return _str == r._str ? 0 : str().compare(r.str()); }
	inline bool operator==(const Symbol& r) const { return _str == r._str;  }
	inline bool operator!=(const Symbol& r) const { return _str != r._str; }
};
#if 0
namespace std {
	template<>
	struct hash<Symbol> {
		size_t operator()(const Symbol& s) const { return s.hash(); }
	};
}
#endif



// a complete tree list used to identify the end part of a tree
// Each given a refrence number instead of having to transfer a bunch of pointers around
// This is mainly for names that contain multipul namesm, like paths components, object trees, etc
// This should cut back on file size as well
class SymbolList : public HashInterface<SymbolList> {
	struct list_ref  {
		// we put string view in here so we can pass the const ref around
// instead of recreating it all the time
// I am sure the compiler would figure it out but eh.
		const Symbol* _list;
		size_t _size;
		size_t _hash;
		list_ref* _next;
		friend class SymbolListTable;
	public:
		list_ref(const Symbol* list, size_t size, size_t hash) : _list(list), _hash(hash), _next(nullptr) {}
		const Symbol* data() const { return _list; }
		size_t size() const { return _size; }
		size_t hash() const { return _hash; }
	};
	static const list_ref* _empty;
	const list_ref* _list;
	friend class SymbolListTable;
public:
	SymbolList() : _list(_empty) {}
	size_t size() const { return _list->size(); }
	size_t hash() const { return _list->hash(); }
	const Symbol* data() const { return _list->data(); }
	int compare(const SymbolList& r) const { return _list->hash() - r._list->hash(); } // save 

};
// Trying to stop my ocd here
// I was going to build a templated function that would
// build a str_ref based on a parm pack but that woudl require alot of work
// and right now this is a symbole library for a scripting engine
// ....mabye latter

#endif