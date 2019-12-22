#pragma once

#include <filesystem>
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
#include <string>
#include <functional>
#include <iostream>
#include <optional>
#include <stack>
#include <mutex>
#include <atomic>
#include <sstream>
#include <algorithm>
#include <optional>
#include <cassert>
#include <string>
#include <forward_list>
#include <sstream>

#include <iostream>
#include <unordered_map>
#include <optional>
#include <memory>
#include <string_view>
#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/unescape.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>
#include <tao/pegtl/string_input.hpp>
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; }; // (1)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;  // (2)

#if 0

// this class encu
class string_t;

class string_builder {
	static constexpr size_t block_size = 1024 * 1024; // we allocate in 1 meg blocks
	using lookup_t =std::unordered_map<std::string_view, size_t>;
	using string_size_t = uint16_t;
	struct helper_t {
		string_size_t size;
		char str[1];
	};

	std::vector<char> _table;
	lookup_t _lookup;
	size_t _current_pos;
	helper_t* _currernt;
	void _resize();
public:
	string_builder();
	void clear_mark();
	void push_char(char c);
	void pop_mark();
	size_t intern_mark();

	size_t intern(const char* str, size_t size);
	size_t intern(const std::string_view& s);

	void push(const char* str, size_t size);
	void push(const std::string_view& s) { push(s.data(), s.size()); }

	std::string_view toString(string_t s) const;

	template<typename BEGIN, typename END>
	void push(BEGIN b, END e) {
		std::for_each(b, e, [&](auto c) { push_char(c); });
	}
	template<typename BEGIN, typename END>
	size_t intern_range(BEGIN b, END e) {
		clear_mark();
		push(b, e);
		return intern_mark();
	}
};


class string_t {
	size_t _handle;
	string_t(size_t handle) : _handle(handle | (0x10 << 24)) {}
public:
	static string_builder& builder() {
		static string_builder _builder;
		return _builder;
	}
	constexpr string_t() : _handle(0) {}
	constexpr string_t(char c) : _handle(c) {}
	constexpr string_t(const char(&s)[1]) : _handle('\0') {} // special case for ""
	constexpr string_t(const char(&s)[2]) : _handle(s[0]) {}
	constexpr string_t(const char(&s)[3]) : _handle(s[0] | (s[1] << 8)) {}
	constexpr string_t(const char(&s)[4]) : _handle(s[0] | (s[1] << 8) | (s[1] << 16)) {}
	string_t(std::string_view s) : _handle(builder().intern(s)) {}
	string_t(const char* str, size_t size) : _handle(builder().intern(str,size)) {}
	operator std::string_view() const {
		return builder().toString(_handle);
	}
	bool operator==(const string_t& r) const { return _handle == r._handle; }
	bool operator!=(const string_t& r) const { return _handle != r._handle; }
	friend struct std::hash<string_t>;
	friend class string_builder;
};

namespace std {
	template<>
	struct hash<string_t> {
		std::hash<size_t> h;
		size_t operator()(const string_t& s) const { return h(s._handle); }
	};
};
#else
#if 0
class string_t : public std::string_view {
	static inline std::string_view _lookup(const std::string_view& sv) {
		static std::unordered_map<std::string_view, std::pair<std::unique_ptr<char>, size_t>> string_table;
		auto it = string_table.find(sv);
		if (it != string_table.end())
			return it->first;
		std::unique_ptr<char> str(new char[sv.size() + 1]);
		sv.copy(str.get(), sv.size());
		std::string_view key(str.get(), sv.size());
		return string_table.emplace(
			key, std::make_pair(std::move(str), sv.size())).first->first;
	}

public:
	string_t() : std::string_view() {}
	string_t(const  string_t& s) : std::string_view(s.data(), s.size()) {}
	string_t(const std::string_view& sv) : std::string_view(_lookup(sv)) {}
	string_t(const char* str, size_t s) : string_t(std::string_view(str, s)) {}
	string_t(const std::string& str) : string_t(std::string_view(str.c_str(), str.size())) {}
	template<size_t N>
	string_t(const char (&str)[N]) : string_t(str,N-1) {}
	explicit operator uint32_t() const { return 0; }
};
#endif

// got to love sfian
#if 0
template<typename C, typename... Args>
constexpr decltype(std::declval().location(std::declval <Args>()...), true) has_location(int) { return true; }
template<typename C, typename...Args>
constexpr bool has_update(...) { return false; }
#endif
// static_assert( has_location<some_object>(0) == true, "we are good")

using string_id_t = uintptr_t;

class string_t {
	string_id_t _index;
	struct string_table {
		std::mutex _mutex;
		std::vector<char> _cache;
		std::unordered_map<std::string_view, string_id_t> _lookup;
		string_table() 
		{ _cache.reserve(1024 * 1024); 
		_cache.push_back(0); 
		_lookup.emplace(std::string_view(&_cache[0], 0), 0);
		}
		string_id_t create(const std::string_view& s) {
			std::lock_guard<std::mutex> lock(_mutex);
			if (auto it = _lookup.find(s); it != _lookup.end())
				return it->second;
			string_id_t index = static_cast<string_id_t>(_cache.size());
			_cache.insert(_cache.end(), s.begin(), s.end());
			_cache.push_back(0); // zero termination
			_lookup.emplace(std::string_view(&_cache[index], s.size()), index);
			return index;
		}
		const char* lookup(string_id_t i) const { return &_cache[i]; }
	};
	static inline string_table& string_list() { static string_table _strings; return _strings; }
	string_t(string_id_t index) :_index(index) {}
public:
	string_t() :_index(0) {}
	string_t(const std::string_view& sv) : _index(string_list().create(sv)) {}
	string_t(const char* str, size_t s) : string_t(std::string_view(str, s)) {}
	string_t(const char* str) : string_t(std::string_view(str)) {}
	string_t(const std::string& str) : string_t(std::string_view(str.c_str(), str.size())) {}
	template<size_t N>
	string_t(const char(&str)[N]) : string_t(str, N - 1) {}
	explicit operator string_id_t() const { return _index; }
	operator std::string_view() const { return string_list().lookup(_index); }
	const char* c_str() const { return string_list().lookup(_index); }
	// very crappy if we are doing multi threads
	class builder_t {
		std::lock_guard<std::mutex> lock;
		string_id_t _start;
	public:
		builder_t() : lock(string_list()._mutex), _start(string_list()._cache.size()) {}
		~builder_t() { clear(); }
		void push_back(int c) { string_list()._cache.push_back(c); }
		void clear() { string_list()._cache.resize(_start); }
		string_t intern() {
			auto& st = string_list();
			size_t len = st._cache.size() - _start;
			if (auto it = st._lookup.find(std::string_view(&st._cache[_start], len)); it != st._lookup.end()) {
				clear();
				return string_t(it->second);
			}
			st._cache.push_back(0); // zero termination
			st._lookup.emplace(std::string_view(&st._cache[_start], len), _start);
			string_id_t index = _start;
			clear();
			return index;
		}

	};
};
static inline bool operator==(const string_t& l, const string_t& r) {
	return static_cast<string_id_t>(l) == static_cast<string_id_t>(r);
}
static inline std::ostream& operator<<(std::ostream& os, const string_t& s) {
	os << static_cast<const std::string_view>(s);
	return os;
}

namespace std {
	template<>
	struct hash<string_t> {
		std::hash<std::string_view> _hash;
		size_t operator()(const string_t& s) const { return _hash(static_cast<const std::string_view>(s)); }
	};
};

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;



class FileId {
public:
	using type = uint16_t;


	constexpr static type FILEID_BUILTINS = 0;
	constexpr static type FILEID_MIN = FILEID_BUILTINS + 1;
	constexpr static type FILEID_BAD = std::numeric_limits<decltype(type{}) > ::max();
	constexpr static type FILEID_MAX = FILEID_BAD - 1;
	constexpr FileId() : _value(FILEID_BAD) {}
	constexpr FileId(const type& v) : _value(v) {}
	constexpr operator type& () { return _value; }
	constexpr operator const type& () const { return _value; }
private:
	type _value;
};
class Location {
	FileId _fileIndex;
	size_t _line;
	size_t _column;
public:
	constexpr Location() : _fileIndex(FileId()), _line(1), _column(1) {}
	constexpr Location(size_t line, size_t column) : _fileIndex(FileId()), _line(line), _column(column) {}
	constexpr auto line() const { return _line; }
	constexpr auto column() const { return _column; }
	constexpr auto fileIndex() const { return _fileIndex; }
	constexpr uint64_t pack() const { return (static_cast<uint64_t>(_fileIndex) << 48) | (static_cast<uint64_t>(_line) << 16) | static_cast<uint64_t>((_column)); }
	inline Location& pred() {
		if (_column) --_column;
		else if (_line) {
			_line--;
			_column = std::numeric_limits<decltype(_column)>::max();
		}
		else if (_fileIndex) {
			_fileIndex--;
			_line = std::numeric_limits<decltype(_line)>::max();
			_column = std::numeric_limits<decltype(_column)>::max();
		}
		return *this;
	}
	inline Location& add_columns(uint16_t count) { _column += count; return *this; }
	inline Location& add_line(uint16_t line) { _line += line; _column = 0;  return *this; }
	inline bool is_builtins() const { return _fileIndex == std::numeric_limits<decltype(_fileIndex)>::max(); }
};

static inline std::ostream& operator<<(std::ostream& os, const Location& l) {
	os << l.line() << ':' << l.column();
	return os;
}

#endif