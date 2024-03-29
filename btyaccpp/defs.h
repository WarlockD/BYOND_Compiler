#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <set>
#include <fstream>
#include <stack>
#include <iostream>
#include <optional>
#include <array>

#include "mstring.h"

/*  machine-dependent definitions			*/
/*  the following definitions are for the Tahoe		*/
/*  they might have to be changed for other machines	*/

/*  MAXCHAR is the largest unsigned character value	*/
/*  MAXSHORT is the largest value of a C short		*/
/*  MINSHORT is the most negative value of a C short	*/
/*  MAXTABLE is the maximum table size			*/
/*  BITS_PER_WORD is the number of bits in a C unsigned	*/
/*  WORDSIZE computes the number of words needed to	*/
/*	store n bits					*/
/*  BIT returns the value of the n-th bit starting	*/
/*	from r (0-indexed)				*/
/*  SETBIT sets the n-th bit starting from r		*/

#define	MAXCHAR		255
#define	MAXSHORT	((int)0x7FFFFFFF)
#define MINSHORT	((int)0x80000000)
#define MAXTABLE	120000


#define BITS_PER_WORD	32
#define LOG2_BPW    5

#define BITS_PER_WORD_1 (BITS_PER_WORD-1)

#define WORDSIZE(n) (((n)+(BITS_PER_WORD_1))/BITS_PER_WORD)
#define BIT(r, n)   ((((r)[(n)>>LOG2_BPW])>>((n)&BITS_PER_WORD_1))&1)
#define SETBIT(r, n) ((r)[(n)>>LOG2_BPW]|=((unsigned)1<<((n)&BITS_PER_WORD_1)))

/* VM: this is a 32-bit replacement for original 16-bit short */
typedef int Yshort;


/*  character names  */

#define	NUL		'\0'    /*  the null character  */
#define	NEWLINE		'\n'    /*  line feed  */
#define	SP		' '     /*  space  */
#define	BS		'\b'    /*  backspace  */
#define	HT		'\t'    /*  horizontal tab  */
#define	VT		'\013'  /*  vertical tab  */
#define	CR		'\r'    /*  carriage return  */
#define	FF		'\f'    /*  form feed  */
#define	QUOTE		'\''    /*  single quote  */
#define	DOUBLE_QUOTE	'\"'    /*  double quote  */
#define	BACKSLASH	'\\'    /*  backslash  */


/* defines for constructing filenames */

static constexpr const char* DEFINES_SUFFIX = "_tab.h";
static constexpr const char* OUTPUT_SUFFIX = "_tab.cpp";
static constexpr const char* CODE_SUFFIX = "_code.cpp";
static constexpr const char* VERBOSE_SUFFIX = ".output";

/* keyword codes */

#define TOKEN 0
#define LEFT 1
#define RIGHT 2
#define NONASSOC 3
#define MARK 4
#define TEXT 5
#define TYPE 6
#define START 7
#define UNION 8
#define IDENT 9


/*  symbol classes  */
enum SCLASS {
	UNKNOWN,
	TERM,
	NONTERM,
	ACTION,
	ARGUMENT,
};



/*  the undefined value  */

#define UNDEFINED (-1)


/*  action codes  */

#define SHIFT 1
#define REDUCE 2


/*  character macros  */

#define IS_IDENT(c)	(isalnum(c) || (c) == '_' || (c) == '.' || (c) == '$')
#define	IS_OCTAL(c)	((c) >= '0' && (c) <= '7')
#define	NUMERIC_VALUE(c)	((c) - '0')


/*  symbol macros  */

#define ISTOKEN(s)	((s) < start_symbol)
#define ISVAR(s)	((s) >= start_symbol)


/*  storage allocation macros  */

#define CALLOC(k,n)	(calloc((unsigned)(k),(unsigned)(n)))
#define	FREE(x)		(free((char*)(x)))
#define MALLOC(n)	(malloc((unsigned)(n)))
#define	NEW(t)		((t*)allocate(sizeof(t)))
#define	NEW2(n,t)	((t*)allocate((unsigned)((n)*sizeof(t))))
#define REALLOC(p,n)	(realloc((char*)(p),(unsigned)(n)))
#define RENEW(p,n,t)	((t*)realloc((char*)(p),(unsigned)((n)*sizeof(t))))


/*  the structure of a symbol table entry  */
#include <forward_list>



struct symbol_t {
	string_t name;
	string_t tag;
	symbol_t(string_t name="",string_t tag="") : name(name), tag(tag) {}
	bool operator==(const string_t& n) const { return name == n; }
	bool operator==(const symbol_t& n) const { return name == n.name; }
};

namespace std {
	template<>
	struct hash<symbol_t>{
		std::hash<string_t> hasher;
		size_t operator()(const symbol_t& n) const { return hasher(n.name); }
		size_t operator()(const string_t& n) const { return hasher(n); }
	};
}

struct bucket_t
{
   // struct bucket *link;
   // struct bucket *next;
	//std::vector<symbol_t> argnames;
	//std::vector<string_t> argtags;
   // Yshort args;
	symbol_t name;
	std::vector<string_t> args;
    Yshort value;
    Yshort index;
    Yshort prec;
	SCLASS  cclass;
    char assoc;
	bucket_t(string_t name) : name(name), value(UNDEFINED), index(0), prec(0), cclass(UNKNOWN), assoc(TOKEN) {}
};


using bucket_list = std::forward_list<bucket_t>;
using bucket = bucket_list::iterator;

/*  the structure of the LR(0) state machine  */

typedef struct core core;
struct core
{
    struct core *next;
    struct core *link;
    Yshort number;
    Yshort accessing_symbol;
    Yshort nitems;
    Yshort items[1];
};


/*  the structure used to record shifts  */

typedef struct shifts shifts;
struct shifts
{
    struct shifts *next;
    Yshort number;
    Yshort nshifts;
    Yshort shift[1];
};


/*  the structure used to store reductions  */

typedef struct reductions reductions;
struct reductions
{
    struct reductions *next;
    Yshort number;
    Yshort nreds;
    Yshort rules[1];
};


/*  the structure used to represent parser actions  */

typedef struct action action;
struct action
{
    struct action *next;
    Yshort symbol;
    Yshort number;
    Yshort prec;
    char   action_code;
    char   assoc;
    char   suppressed;
};

struct section {
	const char   *name;
	const char  **ptr;
};

extern  struct section section_list[];


/* global variables */

extern char dflag;
extern char lflag;
extern char rflag;
extern char tflag;
extern char vflag;

extern char *myname;
extern char *cptr;
extern char *line;
extern int lineno;
extern int outline;

extern const char *banner[];
extern const char *tables[];
extern const char *header[];
extern const char *body[];
extern const char *trailer[];

extern std::filesystem::path action_file_name;
extern std::filesystem::path code_file_name;
extern std::filesystem::path defines_file_name;
extern std::filesystem::path input_file_name;
extern std::filesystem::path output_file_name;
extern std::filesystem::path text_file_name;
extern std::filesystem::path union_file_name;
extern std::filesystem::path verbose_file_name;



class include_stack_t  {  
	struct info_t {
		std::ifstream file;
		std::filesystem::path path;
		size_t lineno;
	};
	std::stack<std::tuple<std::filesystem::path,size_t, std::ifstream>> _stack;
	std::string _line_buffer;

public:
	include_stack_t() {}
	inline bool eof() const { return _stack.empty() ? true : false; }
	inline const std::filesystem::path& path() const { return std::get<0>(_stack.top()); }
	inline size_t lineno() const { return std::get<1>(_stack.top()); }
	bool getline(std::string& line) {
		std::string ret;
		while (eof()) {
			auto& f = std::get<2>(_stack.top());
			auto & lineno = std::get<1>(_stack.top());
			if (f.eof() || f.bad()) {
				_stack.pop();
				continue;
			}
			std::getline(f, ret);
			std::get<1>(_stack.top())++;
			return true;
		}
		return false;
	}
	void push(std::filesystem::path path) {
		_stack.emplace(path, 1, path);

	}
};



extern std::ofstream action_file;
extern std::ofstream code_file;
extern std::ofstream defines_file;
extern std::ifstream input_file;
extern std::ofstream output_file;
extern std::ofstream text_file;
extern std::ofstream union_file;
extern std::ofstream verbose_file;

extern int nitems;
extern int nrules;
extern int nsyms;
extern int ntokens;
extern int nvars;
extern int ntags;

extern char unionized;
extern char line_format[];

extern int   start_symbol;
extern char  **symbol_name;
extern Yshort *symbol_value;
extern Yshort *symbol_prec;
extern char  *symbol_assoc;

extern Yshort *ritem;
extern Yshort *rlhs;
extern Yshort *rrhs;
extern Yshort *rprec;
extern char  *rassoc;

extern Yshort **derives;
extern char *nullable;

extern bucket *first_symbol;
extern bucket *last_symbol;

extern int nstates;
extern core *first_state;
extern shifts *first_shift;
extern reductions *first_reduction;
extern Yshort *accessing_symbol;
extern core **state_table;
extern shifts **shift_table;
extern reductions **reduction_table;
extern unsigned *LA;
extern Yshort *LAruleno;
extern Yshort *lookaheads;
extern Yshort *goto_map;
extern Yshort *from_state;
extern Yshort *to_state;

extern action **parser;
extern int SRtotal;
extern int RRtotal;
extern Yshort *SRconflicts;
extern Yshort *RRconflicts;
extern Yshort *defred;
extern Yshort *rules_used;
extern Yshort nunused;
extern Yshort final_state;


/* global functions */

/* closure.c */
void set_EFF(void);
void set_first_derives(void);
void closure(Yshort *, int);
void finalize_closure(void);
void print_closure(int);
void print_EFF(void);
void print_first_derives(void);

/* error.c */
void fatal(const char *);
void no_space(void);
void open_error(const std::filesystem::path& path);
void unexpected_EOF(void);
void print_pos(char *, char *);
void error(int, char *, char *, const char *, ...);
void syntax_error(int, char *, char *);
void unterminated_comment(int, char *, char *);
void unterminated_string(int, char *, char *);
void unterminated_text(int, char *, char *);
void unterminated_union(int, char *, char *);
void over_unionized(char *);
void illegal_tag(int, char *, char *);
void illegal_character(char *);
void used_reserved(char *);
void tokenized_start(char *);
void retyped_warning(char *);
void reprec_warning(char *);
void revalued_warning(char *);
void terminal_start(char *);
void restarted_warning(void);
void no_grammar(void);
void terminal_lhs(int);
void prec_redeclared(void);
void unterminated_action(int, char *, char *);
void unterminated_arglist(int, char *, char *);
void bad_formals(void);
void dollar_warning(int, int);
void dollar_error(int, char *, char *);
void untyped_lhs(void);
void untyped_rhs(int, char *);
void unknown_rhs(int);
void default_action_warning(void);
void undefined_goal(char *);
void undefined_symbol_warning(char *);

/* lalr.c */
void lalr(void);
void set_state_table(void);
void set_accessing_symbol(void);
void set_shift_table(void);
void set_reduction_table(void);
void set_maxrhs(void);
void initialize_LA(void);
void set_goto_map(void);
int map_goto(int, int);
void initialize_F(void);
void build_relations(void);
void add_lookback_edge(int, int, int);
Yshort **transpose(Yshort **, int);
void compute_FOLLOWS(void);
void compute_lookaheads(void);
void digraph(Yshort **);
void traverse(int);

/* lr0.c */
void allocate_itemsets(void);
void allocate_storage(void);
void append_states(void);
void free_storage(void);
void generate_states(void);
int get_state(int);
void initialize_states(void);
void new_itemsets(void);
core *new_state(int);
void show_cores(void);
void show_ritems(void);
void show_rrhs(void);
void show_shifts(void);
void save_shifts(void);
void save_reductions(void);
void set_derives(void);
void free_derives(void);
void print_derives(void);
void set_nullable(void);
void free_nullable(void);
void lr0(void);

/* main.c */
void done(int);
void onintr(void);
void set_signals(void);
void usage(void);
void getargs(int, char **);
char *allocate(unsigned);
void create_file_names(void);
void open_files(void);
int main(int, char **);

/* mkpar.c */
void make_parser(void);
action *parse_actions(int);
action *get_shifts(int);
action *add_reductions(int, action *);
action *add_reduce(action *, int, int);
void find_final_state(void);
void unused_rules(void);
void remove_conflicts(void);
void total_conflicts(void);
int sole_reduction(int);
void defreds(void);
void free_action_row(action *);
void free_parser(void);

/* output.c */
void output(void);
void output_rule_data(void);
void output_yydefred(void);
void output_actions(void);
int find_conflict_base(int);
void token_actions(void);
void goto_actions(void);
int default_goto(int);
void save_column(int, int);
void sort_actions(void);
void pack_table(void);
int matching_vector(int);
int pack_vector(int);
void output_base(void);
void output_table(void);
void output_check(void);
void output_ctable(void);
int is_C_identifier(char *);
void output_defines(void);
void output_stored_text(void);
void output_debug(void);
void output_stype(void);
void output_trailing_text(void);
void output_semantic_actions(void);
void free_itemsets(void);
void free_shifts(void);
void free_reductions(void);
void write_section(const char * section_name);


/* reader.c */
int cachec(int);
std::string get_line(void);
char *dup_line(void);
char *skip_comment(void);
int nextc(void);
int keyword(void);
void copy_ident(void);
void copy_string(int, FILE *, FILE *);
void copy_comment(FILE *, FILE *);
void copy_text(void);
void copy_union(void);
int hexval(int);
bucket *get_literal(void);
int is_reserved(char *);
bucket *get_name(void);
int get_number(void);
char *get_tag(void);
void declare_tokens(int);
void declare_types(void);
void declare_start(void);
void read_declarations(void);
void initialize_grammar(void);
void expand_items(void);
void expand_rules(void);
void advance_to_start(void);
void start_rule(bucket *, int);
void end_rule(void);
void insert_empty_rule(void);
void add_symbol(void);
void copy_action(void);
int mark_symbol(void);
void read_grammar(void);
void free_tags(void);
void pack_names(void);
void check_symbols(void);
void pack_symbols(void);
void pack_grammar(void);
void print_grammar(void);
void reader(void);

/* readskel.c */
void read_skel(const char *);

/* symtab.c */
bucket make_bucket(string_t name);
bucket lookup(string_t name);

/* verbose.c */
void verbose(void);
void log_unused(void);
void log_conflicts(void);
void print_state(int);
void print_conflicts(int);
void print_core(int);
void print_nulls(int);
void print_actions(int);
void print_shifts(action *);
void print_reductions(action *, int);
void print_gotos(int);

/* warshall.c */
void transitive_closure(unsigned *, int);
void reflexive_transitive_closure(unsigned *, int);
