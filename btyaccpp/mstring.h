#ifndef _string_h_
#define _string_h_

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>

class string_t {
	static constexpr std::string_view empty_string = std::string_view();
	std::string_view _value;
	static std::string_view _lookup(std::string_view sv);
public:
	constexpr string_t() : _value(nullptr) {}


	string_t(const string_t& copy) = default;
	string_t(std::string_view sv) : _value(_lookup(sv)) {}
	string_t(const char* s, size_t l) : string_t(_lookup(std::string_view(s, l))) {}
	template<size_t N> // captures string literals
	string_t(const char (&s)[N]) : string_t(s,N-1) {}
	string_t(const std::string& str) : string_t(_lookup(str)) {}
	std::string_view str() const { return _value; }

	operator std::string_view() const { return _value; }

	bool operator==(const string_t& s) const { return _value.data() == s._value.data(); }
	bool operator!=(const string_t& s) const { return _value.data() != s._value.data(); }
	bool operator!() const { return _value == empty_string; }
};

namespace std {
	template<>
	struct hash<string_t> {
		hash<std::string_view> hasher;
		size_t operator()(const string_t& s) const { return hasher(s); }
		size_t operator()(const std::string_view& s) const { return hasher(s); }
	};

};

#if 0
struct mstring {
    char	*base, *ptr, *end;
    };

void msprintf(struct mstring *, const char *, ...);
int mputchar(struct mstring *, int);
struct mstring *msnew(void);
char *msdone(struct mstring *);
#endif
/* compare two strings, ignoring whitespace, except between two letters or
** digits (and treat all of these as equal) */
int strnscmp(const char *, const char *);
/* hash a string, ignoring whitespace */
unsigned int strnshash(const char *);

#define mputc(m, ch)	((m)->ptr==(m)->end?mputchar(m,ch):(*(m)->ptr++=(ch)))


#endif /* _string_h_ */
