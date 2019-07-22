#pragma once


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

#include <sstream>

using string_t = std::shared_ptr<std::string>;

static inline constexpr int toKind(char c) { return static_cast<uint8_t>(c); }
static inline constexpr int toKind(const char c[2]) { return (static_cast<uint8_t>(c[1]) << 8) || static_cast<uint8_t>(c[0]); }
static inline constexpr int toKindIdent(size_t i,char c = ' ') { return static_cast<uint16_t>(c)<< 16 | static_cast<uint8_t>(c); }
static inline constexpr uint16_t getKind(int i) { return static_cast<uint16_t>(i); }
static inline constexpr uint16_t getIdent(int i) { return static_cast<uint16_t>(i >> 16); }

class FilePosition {
	string_t _filename;
	size_t _line;
	size_t _pos;
public:
	FilePosition(string_t filename, size_t line, size_t pos) : _filename(filename), _line(line), _pos(pos) {}
	size_t pos() const { return _pos; }
	size_t lineno() const { return _line; }
	const string_t& filename() const { return _filename; }
	bool operator==(const FilePosition& r) const { return _filename == r._filename && _line == r._line && _pos == r._pos; }
	friend class Lexer;
};

class Token {
	int _kind;
	std::string_view _str;
	FilePosition _fpos;
public:
	Token(int kind, std::string_view str, FilePosition fpos) : _kind(kind), _str(str), _fpos(fpos) {}
	int kind() const { return _kind; }
	std::string_view str() const { return _str; }
	const FilePosition& pos() const { return _fpos; }
	bool operator==(const int l) const { return _kind == l ; }
	bool operator!=(const int l) const { return _kind != l; }
};

class File {
	std::unique_ptr<std::istream> _ss;
	string_t _filename; // or macro
	size_t _lineno;
	template<typename T>
	File(T* p, string_t filename) : _ss(std::unique_ptr<std::istream>(p)), _filename(filename), _lineno(0){}
public:
	File() : _ss(nullptr), _filename(nullptr), _lineno(0){}
	static File open_file(string_t filename) {
		std::fstream* f = new std::fstream(*filename, std::ios::in);
		if (!f->good()) {
			delete f;
			throw 0; // fix this
		}
		return std::move(File(f, filename));
	}
	static File open_string(string_t s) {
		return std::move(File(new std::istringstream(*s),nullptr));
	}
	bool is_file() const { return _filename != nullptr; }
	string_t filename() const { return _filename; }
	operator std::istream& () { return *_ss; }
	std::istream* operator->() { return _ss.get(); }
	std::optional<size_t> read_line(std::string& line) {
		while (_ss->eof() || _ss->bad()) {
			int ch;
			bool line_empty = true;
			_lineno++;
			while ((ch = _ss->get()) != -1 && (ch != '\r') && (ch != 'n')) {
				if (!isspace(ch)) line_empty = false;
				line.push_back(ch);
			}
			int peek = _ss->peek();
			if (ch != peek && (peek == '\r' || peek == '\n'))
				_ss->get();// skip windows line endings
			if (!line_empty) break; // skip blank lines
		}
		if (line.empty()) return std::nullopt;
		else return _lineno;
	}
};
using tokenrow_t = std::vector<Token>;

class Lexer {
	// Ok, kind of meh, but its a quick and dirrty symbol table
	std::unordered_map<std::string_view, string_t> _symbols;
	std::stack<File> _fileStack;
	// 
	string_t lookup(std::string_view s) {
		auto it = _symbols.find(s);
		if (it != _symbols.end()) return it->second;
		auto str = std::make_shared<std::string>(s);
		_symbols.emplace(std::pair(*str, str));
		return str;
	}
	struct Define {
		string_t name;
		std::vector<string_t> _args;
		std::vector< Token> line;
	};
	size_t _ifStack;

	bool read_row(tokenrow_t& tokens) {
#if 0
		std::string line;
		while(_fileStack.empty()) {
			File& file = _fileStack.top();
			auto lineno = file.read_line(line);
			if (lineno.has_value()) {
				FilePosition pos(file.filename(), lineno.value(), 0);



			}
		} while (line.empty());
#endif
		return false;
	}
public:
};


#if 0
namespace _private {

	template<typename T>
	int ctrans(T& ss)
	{
		int c = *ss;
		if (c == '\\') {
			int k = 0;
			switch (c = *++ss)
			{
			case 'n': return '\n';
			case 't': return'\t';
			case 'r': return '\r';
			case 'b':return '\b';
			case 'f': return '\f'; 		/* form feed for ascii */
			case '\\': return '\\';
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7': {
				int k;
				c -= '0';
				while ((k = (*ss + 1)) >= '0' && k <= '7')
				{
					c = c * 8 + k - '0';
					++ss;
				}
				break;
			}
			};
		}
		return c;
	}
	int usescape(int c) {
		int d;
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
	template<typename K, typename L>
	std::optional<size_t> lookup(const K& key, const L& list) {
		for (size_t i = 0; i < list.size(); i++) {
			if (list.at(i) == key) return i;
		}
		return nullptr;
	}
};
#endif