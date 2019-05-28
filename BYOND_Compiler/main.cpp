#include <iostream>
#include <fstream>
#include <filesystem>
#include <type_traits>
#include <stack>
#include <filesystem>

#include "Symbol.h"
#include <unordered_map>
#include <variant>
#include "CPPLex.hpp"
#include <queue>
#include <mcpp_lib.h>

class FileReader {
	
	std::vector<char> _data;
	std::vector<std::string_view> _lines;
	size_t _lineno;
	size_t _pos;
	Symbol _filename;  // absolute file name?
	std::ifstream _handle;
	std::string _line;
	int ch; 
public:
	FileReader(const std::string_view& file_name) :_filename(file_name), _lineno(0) {
		_handle.open(_filename.data());
		assert(_handle.good() && _handle.is_open());
		while (!_handle.eof()) {
			char buffer[1024];
			_handle.read(buffer, 1024);

		}
	}
	int readline(std::string& line) {
		assert(_handle.good() && _handle.is_open());
		std::getline(_handle, line);
		if (_handle.eof() || _handle.bad() || _handle.fail()) return -1;
		return (int)++_lineno;
	}

	std::filesystem::path path() const { return std::filesystem::path(_filename.str()).parent_path(); }
};
Symbol tag_define("#define");
Symbol tag_include("#include");
std::stack <std::unique_ptr<FileReader>> includes;

std::unordered_map<Symbol, std::variant<void*, int, Symbol, SymbolList>> _macros;

static const char* root_dir = "E:\\GitHub\\FTL13\\ftl13.dme";
int main() {
	PascalLexer::Scanner scanner("[A-Za-z][A-Za-z0-9_]*");
	PascalLexer::Parser p;


	auto rule = p.parse_rule(1, "\\#?[A-Za-z][A-Za-z0-9_]*");

#if 0
	extern DLL_DECL int     mcpp_lib_main(int argc, char ** argv);
	extern DLL_DECL void    mcpp_reset_def_out_func(void);
	extern DLL_DECL void    mcpp_set_out_func(
		int(*func_fputc)  (int c, OUTDEST od),
		int(*func_fputs)  (const char * s, OUTDEST od),
		int(*func_fprintf)(OUTDEST od, const char * format, ...)
	);

	extern DLL_DECL void    mcpp_use_mem_buffers(int tf);
	extern DLL_DECL char *  mcpp_get_mem_buffer(OUTDEST od);
#endif
	std::cout << *rule.nodes;
	//std::cout << "Wecomd to hell " << test_one << ", " << test_two << " " << std::endl;
	std::cout << "we are done!" << std::endl;
}