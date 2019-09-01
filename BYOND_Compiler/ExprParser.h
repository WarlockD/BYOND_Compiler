#pragma once

#include "util.hpp"


namespace byond_compiler {
	// copyed from the lua53 example parser
	using namespace TAO_PEGTL_NAMESPACE;  // NOLINT
	struct blank : sor<one<' '>, one<'\t'>> {}; // pegtl space includes \n\r
	struct star_blank : star<blank> {}; // pegtl space includes \n\r
	struct plus_blank : plus<blank> {};

			// clang-format off
	struct _line_comment : seq< one< '/' >, not_at< eol > > {}; // We don't want to  consume the end of line, but pass it to the parser
	struct _multiline_comment : seq< one< '*'>, until< string< '*', '/'> ,any > > {};
	struct comment : seq<one< '/' >, sor< _line_comment, _multiline_comment > > {};

	struct line_comment : seq< two< '/' >, not_at< eol > > {};
	struct multiline_comment : seq< string<'/', '*'>, until< string< '*', '/'>, any > > {};

	
	struct sep : sor<star_blank,  comment > {};
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
	struct literal_char : short_string< '\'' > {};

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

		struct DEFINE : key< str_define > {};
		struct UNDEF : key< str_undef > {};
		struct IFDEF : key< str_ifdef > {};
		struct IFNDEF : key< str_ifndef > {};
		struct IF : key< str_if > {};
		struct ELSE : key< str_else > {};
		struct ENDIF : key< str_endif > {};
		struct DEFINED : key< str_defined > {};
		struct KEYWORDS : key< str_keyword > {}; // all of them

		struct name : seq< not_at< KEYWORDS >, identifier > {};
		struct name_list : list< name, one< ',' >, blank > {};
		struct name_list_must : list_must< name, one< ',' >, blank > {};


#if 0
		struct parameter_list_one : seq < name_list, opt_must< pad< one< ',' >, ellipsis > > {};
		struct parameter_list : sor< ellipsis, parameter_list_one > {};
#endif
		template< typename R, typename P = blank >
		struct pad_blank:  seq< R, star< P > >{};
		struct begin_cpp : pad_blank<one<'#'>> {};

		struct hash : one<'#'> {}; // We are in the start of a preprocessor statment
		struct double_hash : two<'#'> {};
		struct macro_tokens : sor<literal_string, name, hash, double_hash, not_one<'\\'>, seps> {};


		struct macro_line : list<not_at<eol>, seq<one<'\\'>, eol>> {};


		struct normal_line : seq< bol, until<eolf> > {};
		struct define_arg_comma : pad_blank<one<','>> {};
		struct define_list_end : pad_blank<one<')'>> {};
		struct define_name : name {};
		struct define_arg : name {};
		struct define_list : seq <one<'('>, star_blank, opt < list_must<define_arg, define_arg_comma>> , define_list_end>{};

		struct preprocessor_commands : seq<
			define_name, sor<star_blank,define_list, macro_line >, // define preprocesso
			until<eol>
		> {};
		//struct preprocessor_start : if_must<seq<hash, star<blank>>, preprocessor_commands> {};

		struct something : seq<bol, sor<  if_must<begin_cpp, preprocessor_commands>, normal_line >> {};
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
				st.temp_macro = macro_t(string_t(in.string()));				// macro starts here so lets clear the state here
			}
		};

		template<>
		struct action< define_arg > {
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

		static inline int test_it(int argc, const char** argv)
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
};

