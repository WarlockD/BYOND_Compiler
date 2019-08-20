#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <optional>
#include <memory>
#include <string_view>
#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/unescape.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

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

namespace byond_compiler {
	// copyed from the lua53 example parser
	using namespace TAO_PEGTL_NAMESPACE;  // NOLINT
			// clang-format off
	struct _line_comment : seq< one< '/' >, until< eolf > > {};
	struct _multiline_comment : seq< one< '*'>, until< string< '*', '/'> > > {};
	struct blank : sor<one<' '>, one<'\t'>> {}; // pegtl space includes \n\r
	struct opt_blank : star<blank> {}; // pegtl space includes \n\r
	struct must_blank : plus<blank> {};
	struct line_comment : disable< seq<star<blank> ,one< '/' >, _line_comment >> {};
	struct multiline_comment : disable< one< '/' >, _multiline_comment > {};


	struct comment : disable< one< '/' >, sor< _line_comment, _multiline_comment > > {};
	struct continue_line : seq< one<'\\'>, eolf> {};// not sure how we handle / for line cuts yet hopefuly this works in all cases?
	struct sep : sor< ascii::space, comment, continue_line> {};
	struct seps : star< sep > {};
	// All above covers any spaces and comments inbetween junk, it all gets cut out


	struct single : one< 'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\'', '0', '\n' > {};
	struct spaces : seq< one< 'z' >, star< space > > {};
	struct hexbyte : if_must< one< 'x' >, xdigit, xdigit > {};
	struct decbyte : if_must< digit, rep_opt< 2, digit > > {};
	struct unichar : if_must< one< 'u' >, one< '{' >, plus< xdigit >, one< '}' > > {};
	struct escaped : if_must< one< '\\' >, sor< hexbyte, decbyte, unichar, single, spaces > > {};
	struct regular : not_one< '\r', '\n' > {};
	struct character : sor< escaped, regular > {};
	// need to figure out how to do long string, how "this " "that" = "this that" even on line endings
	// or does it matter in byond? humm

	template< char Q >
	struct short_string : if_must< one< Q >, until< one< Q >, character > > {};
	struct literal_string : short_string< '"' > {};
	struct literal_char : short_string< '\'' >  {};

	template< typename E >
	struct exponent : opt_must< E, opt< one< '+', '-' > >, plus< digit > > {};

	template< typename D, typename E >
	struct numeral_three : seq< if_must< one< '.' >, plus< D > >, exponent< E > > {};
	template< typename D, typename E >
	struct numeral_two : seq< plus< D >, opt< one< '.' >, star< D > >, exponent< E > > {};
	template< typename D, typename E >
	struct numeral_one : sor< numeral_two< D, E >, numeral_three< D, E > > {};

	struct decimal : numeral_one< digit, one< 'e', 'E' > > {};
	struct hexadecimal : if_must< istring< '0', 'x' >, numeral_one< xdigit, one< 'p', 'P' > > > {};
	struct numeral : sor< hexadecimal, decimal > {};

	namespace preprocessor {


		// Since the the byond has NO ENUMS  OR ANYTHING A PROPER LANGUAGE SHOULD HAVE FOR ITS CONSTANTS ahem
		// (rant over)
		// It uses a c preprocessor to handle all that.  Because its so important for the laguage to use the preprocessor
		// we really have to connect the two, but because I am still learning pegtl, we are seeperating them for now.

		struct str_define : TAO_PEGTL_STRING("define") {};
		struct str_undef : TAO_PEGTL_STRING("undef") {};
		struct str_ifdef : TAO_PEGTL_STRING("ifdef") {};
		struct str_ifndef : TAO_PEGTL_STRING("ifndef") {};
		struct str_if : TAO_PEGTL_STRING("if") {};
		struct str_else : TAO_PEGTL_STRING("else") {};
		struct str_endif : TAO_PEGTL_STRING("endif") {};
		struct str_defined : TAO_PEGTL_STRING("defined") {};
		struct str_keyword : sor< str_defined, str_define, str_ifndef, str_ifdef, str_if, str_else, str_defined> {};
		template< typename Key >
		struct key : seq< Key, not_at< identifier_other > > {};

		struct key_define : key< str_define > {};
		struct key_undef : key< str_undef > {};
		struct key_ifdef : key< str_ifdef > {};
		struct key_ifndef : key< str_ifndef > {};
		struct key_if : key< str_if > {};
		struct key_else : key< str_else > {};
		struct key_endif : key< str_endif > {};
		struct key_defined : key< str_defined > {};
		struct key_keyword : key< str_keyword > {}; // all of them

		struct name : seq< not_at< key_keyword >, identifier > {};
		struct name_list : list< name, one< ',' >, blank > {};
		struct name_list_must : list_must< name, one< ',' >, blank > {};


#if 0
		struct parameter_list_one : seq < name_list, opt_must< pad< one< ',' >, ellipsis > > {};
		struct parameter_list : sor< ellipsis, parameter_list_one > {};
#endif

		struct hash : one<'#'> {}; // We are in the start of a preprocessor statment
		struct double_hash : two<'#'> {};
		struct macro_tokens : sor<literal_string, name, hash, double_hash, not_one<'\\'>, seps> {};
		struct macro_line : until<eol, macro_tokens> {};

		struct macro_lines : if_must< seq<one< '\\' >, eol>, seq<one< '\\' >, eol>, macro_line > {};

		//until<sor<disable<string<'/','/'>>,eolf>> {}; //  until < star<sor<line_comment, eolf> >>{};
		struct define_assignment : seq<must_blank, name, must_blank, macro_lines> {};

		struct macro_arg : name {};
		struct macro_list : list< macro_arg, one< ',' >, blank > {};
		struct macro_arguments : if_must< macro_list, one<')'>> {};
		struct define_macro_arguments : if_must<one<'('>, macro_arguments> {};
		struct define_name : name {};
		struct define_statment : seq<hash, key_define, plus<blank>, define_name, sor<define_macro_arguments, star<blank>>, macro_line> {};

		struct normal_line : seq< bol, until<eolf> > {};
		struct something : seq<bol, sor<  define_statment, normal_line >> {};
		struct grammar : until< eof, must< something > > {};

		// symbol table for the preprocessor
		using opt_string = std::optional<std::string>;

	
		struct macro_t {
			string_t name;
			opt_string value;
			std::vector<string_t> args;
			macro_t() : name(), value(std::nullopt), args() {}
			macro_t(string_t name, opt_string value) : name(name), value(value) {}
			macro_t(string_t name) : name(name), value(std::nullopt) {}
		};
		struct symbol_table_t {

		};
		struct state
		{
			macro_t temp_macro;
			std::unordered_map< string_t, macro_t> symbol_table;
		};

		template< typename Rule > struct action {};
		template<>
		struct action< define_name > {
			template< typename Input >
			static void apply(const Input& in, state& st)
			{
				st.temp_macro = macro_t(string_t(in.string()));
				std::cout << '(' << st.temp_macro.name << ' ';
			}
		};
		template<>
		struct action< macro_arg > {
			template< typename Input >
			static void apply(const Input& in, state& st)
			{
				st.temp_macro.args.push_back(in.string());
				//std::cout << '(' << st.temp_name << ' ';
			}
		};
		template<>
		struct action< macro_line >
		{
			template< typename Input >
			static void apply(const Input& in, state& st)
			{
				size_t startpos = in.string().find_first_not_of(" \t\f\n\r");
				size_t endpos = in.string().find_last_not_of(" \t\f\n\r");
				if (startpos == std::string::npos) {
					st.temp_macro.value = std::nullopt;
				} 
				else {
					std::string str = in.string();
					str = str.substr(0, endpos + 1);
					str = str.substr(startpos);
					st.temp_macro.value = str;
				}
				st.symbol_table.emplace(st.temp_macro.name, st.temp_macro);
			//	std::cout << " In table" << std::endl;
			}
		};
		template<>
		struct action< define_assignment  >
		{
			template< typename Input >
			static void apply(const Input& in, state& st)
			{
				//auto i = st.symbol_table.emplace(st.temp_name, st.temp_value);
				//std::cout << " In table" << std::endl;
#if 0
				if (!st.symbol_table.end().second) {
					throw parse_error("redefining symbol " + st.temp_name, in);  // NOLINT
				}
#endif
			}
		};

	};
	static inline int test_it(int argc, char** argv)
	{
		const std::size_t issues_found = tao::pegtl::analyze<  preprocessor::grammar >();
		for (int i = 1; i < argc; ++i) {
			file_input in(argv[i]);
			preprocessor::state st;
			parse< preprocessor::grammar, preprocessor::action >(in, st);
			for (const auto& j : st.symbol_table) {
				std::cout << j.first;
				if (j.second.args.size() > 0) {
					std::cout << '(';
					bool comma = true;
					for (const auto& a : j.second.args) {
						if (comma) { comma = false; }
						else std::cout << ',';
						std::cout << a;
					}
					std::cout << ')';
				}
				std::string val = j.second.value.value_or(std::string(""));
				std::cout << '=' << val << std::endl;
			}
		}
		return 0;
	}
};



