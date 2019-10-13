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

#include <iostream>
#include <unordered_map>
#include <optional>
#include <memory>
#include <string_view>
#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/unescape.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>
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
};

static inline bool operator==(const string_t& l, const string_t& r) {
	return l.data() == r.data();
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

#endif