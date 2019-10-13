/* 
 * routines for printing error messages  
 */

#include "defs.h"
#include <stdarg.h>
#include <iostream>

extern std::fstream inc_file;
extern std::filesystem::path inc_file_name;

/*
 * VM: print error message with file coordinates.
 * Do it in style acceptable to emacs.
 */
class FileError {
	char _message[512];
public:
	FileError(const char* m, ...) {
		va_list va;
		va_start(va, m);
		vsnprintf(_message, sizeof(_message), m, va);
		va_end(va);
	}
	const char* msg() const { return _message;  }
};
static inline std::ostream& operator<<(std::ostream& os, const FileError& f)  {
	if (inc_file.is_open())
		os << inc_file_name;
	else
		os << input_file_name;
	os << ':' << lineno << ": ";
	os << f.msg() << std::endl;
	return os;
}


__declspec(noreturn) void no_space() { fatal("out of space"); }

__declspec(noreturn)
void open_error(const std::filesystem::path& filename)
{
	std::cerr << "fatal - cannot open \"" << filename << "\"" << std::endl;
    done(2);
}

__declspec(noreturn)
void unexpected_EOF()
{
  std::cerr << FileError("unexpected end-of-file");
  done(1);
}

class print_pos {
	const char* st_line;
	const char* st_cptr;
public:
	print_pos(const char* line, const char* cptr) :st_line(line), st_cptr(cptr) {}
	void print(std::ostream& os) const {
		if (st_line == 0) return;
		for (const char*s = st_line; *s != '\n'; ++s)
		{
			if (isprint(*s) || *s == '\t')
				putc(*s, stderr);
			else
				putc('?', stderr);
		}
		putc('\n', stderr);
		for (const char* s = st_line; s < st_cptr; ++s)
		{
			if (*s == '\t')
				putc('\t', stderr);
			else
				putc(' ', stderr);
		}
		putc('^', stderr);
		putc('\n', stderr);
	}

	
};
std::ostream& operator<<(std::ostream& os, const print_pos& p) {
	p.print(os);
	return os;
}

int read_errs = 0;


void errorv(int lineno, const char* line, const char* cptr, const char* msg, va_list va)
{
	char sbuf[512];
	vsprintf(sbuf, msg, va);
	std::cerr << FileError(sbuf);
	std::cerr << print_pos(line, cptr);
	read_errs++;
}

void error(int lineno, const char * line, const char * cptr, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  errorv(lineno, line, cptr, msg,args);
  va_end(args);
}

void error(int lineno, const std::string& line, const char* cptr, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	errorv(lineno, line.c_str(), cptr, msg,args);
	va_end(args);
}
void error(int lineno,  const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	errorv(lineno, nullptr, nullptr,msg, args);
	va_end(args);
}

__declspec(noreturn)
void syntax_error(int lineno, const char *line, const  char *cptr) {
  error(lineno, line, cptr, "syntax error"); 
  exit(1);
}
__declspec(noreturn)
void unterminated_comment(int lineno, const  char *line, const char *cptr) {
  error(lineno, line, cptr, "unmatched /*"); 
  exit(1);
}
__declspec(noreturn)
void unterminated_string(int lineno, const char *line, const  char *cptr) {
  error(lineno, line, cptr, "unterminated string"); 
  exit(1);
}
__declspec(noreturn)
void unterminated_text(int lineno, const  char *line, const  char *cptr) {
  error(lineno, line, cptr, "unmatched %%{"); 
  exit(1);
}
__declspec(noreturn)
void unterminated_union(int lineno, const char *line, const  char *cptr) {
  error(lineno, line, cptr, "unterminated %%union"); 
  exit(1);
}
__declspec(noreturn)
void over_unionized(const char *cptr) {
  error(lineno, line, cptr, "too many %%union declarations"); 
  exit(1);
}
__declspec(noreturn)
void illegal_tag(int lineno, const  char *line, const  char *cptr) {
  error(lineno, line, cptr, "illegal tag"); 
}
__declspec(noreturn)
void illegal_character(const char *cptr) {
  error(lineno, line, cptr, "illegal character"); 
}
__declspec(noreturn)
void used_reserved(const char *s) {
  error(lineno,  "illegal use of reserved symbol %s", s); 
}
__declspec(noreturn)
void tokenized_start(const char *s) {
  error(lineno, "the start symbol %s cannot be declared to be a token", s); 
}

void retyped_warning(const char *s) {
	std::cerr << FileError("the type of %s has been redeclared", s);
}


void reprec_warning(const char *s) {
	std::cerr << FileError("the precedence of %s has been redeclared", s);
}


void revalued_warning(const char *s) {
	std::cerr << FileError("the value of %s has been redeclared", s);
}

__declspec(noreturn)
void terminal_start(const char *s) {
  error(lineno,  "the start symbol %s is a token", s); 
}

void restarted_warning() {
	std::cerr << FileError("the start symbol has been redeclared");
}
__declspec(noreturn)
void no_grammar() { 
  error(lineno,  "no grammar has been specified"); 
}
__declspec(noreturn)
void terminal_lhs(int lineno) { 
  error(lineno,  "a token appears on the lhs of a production"); 
}
__declspec(noreturn)
void prec_redeclared() { 
  error(lineno, "conflicting %%prec specifiers"); 
}
__declspec(noreturn)
void unterminated_action(int lineno, const char *line, const char *cptr) {
  error(lineno, line, cptr, "unterminated action"); 
}
__declspec(noreturn)
void unterminated_arglist(int lineno, const  char *line, const char *cptr) {
  error(lineno, line, cptr, "unterminated argument list"); 
}
__declspec(noreturn)
void bad_formals() { 
  error(lineno,  "bad formal argument list"); 
}

void dollar_warning(int a_lineno, int i) {
  int slineno = lineno;
  lineno = a_lineno;
  std::cerr << FileError("") << i << " references beyond the end of the current rule";
  lineno = slineno;
}

void dollar_error(int lineno, const char *line, const  char *cptr) {
  error(lineno, line, cptr, "illegal $-name"); 
}

void untyped_lhs() { 
  error(lineno, "$$ is untyped"); 
}

void untyped_rhs(int i, const  char *s) {
  error(lineno,  "$%d (%s) is untyped", i); 
}

void unknown_rhs(int i) { 
  error(lineno,  "$%d is untyped (out of range)", i); 
}

void default_action_warning() {
	std::cerr << FileError("the default action assigns an undefined value to $$");
}

void undefined_goal(const char *s) {
  error(lineno,  "the start symbol %s is undefined", s); 
}

void undefined_symbol_warning(const char *s) {
  fprintf(stderr, "warning - the symbol %s is undefined\n", s);
}
