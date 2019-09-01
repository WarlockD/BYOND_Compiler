#include "defs.h"
#include <signal.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>

char dflag;
char lflag;
char rflag;
char tflag;
char vflag;
bool use_current_dir_as_temp = true;
int Eflag = 0;

std::filesystem::path file_prefix = std::string_view("y");
std::filesystem::path myname = std::string_view("yacc");
std::filesystem::path temp_form = std::string_view("yacc_t_XXXXXX");

int lineno;
int outline;

std::filesystem::path action_file_name;
std::filesystem::path code_file_name;
std::filesystem::path defines_file_name;
std::filesystem::path input_file_name;
std::filesystem::path output_file_name;
std::filesystem::path text_file_name;
std::filesystem::path union_file_name;
std::filesystem::path verbose_file_name;


std::unique_ptr<std::ifstream> input_file = nullptr;	/*  the input file

std::ofstream  action_file ;	/*  a temp file, used to save actions associated    */
			/*  with rules until the parser is written	    */
std::ofstream code_file ;	/*  y.code.c (used when the -r option is specified) */
std::ofstream defines_file ;	/*  y.tab.h					    */
std::ofstream output_file ;	/*  y.tab.c					    */
std::ofstream text_file ;	/*  a temp file, used to save text until all	    */
			/*  symbols have been defined			    */
std::ofstream union_file ;	/*  a temp file, used to save the union		    */
			/*  definition until all symbol have been	    */
			/*  defined					    */
std::ofstream verbose_file ;	/*  y.output					    */

int nitems;
int nrules;
int nsyms;
int ntokens;
int nvars;

int   start_symbol;
char  **symbol_name;
Yshort *symbol_value;
Yshort *symbol_prec;
char  *symbol_assoc;

Yshort *ritem;
Yshort *rlhs;
Yshort *rrhs;
Yshort *rprec;
char  *rassoc;
Yshort **derives;
char *nullable;


void done(int k)
{
	if (action_file.is_open()) { action_file.close(); std::filesystem::remove(action_file_name); }
    if (text_file.is_open()) { text_file.close(); std::filesystem::remove(text_file_name); }
    if (union_file.is_open()) { union_file.close(); std::filesystem::remove(union_file_name); }
    exit(k);
}


void onintr()
{
    done(1);
}

#if 0
// C++ has throw bitch
void set_signals()
{
#ifdef SIGINT
    if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	signal(SIGINT, onintr);
#endif
#ifdef SIGTERM
    if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	signal(SIGTERM, onintr);
#endif
#ifdef SIGHUP
    if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	signal(SIGHUP, onintr);
#endif
}
#endif

void usage()
{
    fprintf(stderr, "usage: %s [-dlrtv] [-b file_prefix] [-S skeleton file] "
		    "filename\n", myname);
    exit(1);
}


void getargs(int argc, char **argv)
{
     int i;
     const char *s;

    if (argc > 0) myname = argv[0];
    for (i = 1; i < argc; ++i)
    {
	s = argv[i];
	if (*s != '-') break;
	switch (*++s)
	{
	case '\0':
		input_file = &std::cin;
	    if (i + 1 < argc) usage();
	    return;

	case '-':
	    ++i;
	    goto no_more_options;

	case 'b':
	    if (*++s)
		 file_prefix = s;
	    else if (++i < argc)
		file_prefix = argv[i];
	    else
		usage();
	    continue;

	case 'd':
	    dflag = 1;
	    break;

	case 'D':
	    /* Find the preprocessor variable */
	    { 
		  extern std::unordered_map<string_t, string_t> defd_vars;
		  std::string var_name;
		  ++s; while (*s && !isspace(*s)) var_name = *++s;
		  if(auto it = defd_vars.find(var_name); it != defd_vars.end())
			  error(lineno, 0, 0, "Preprocessor variable %s already defined", var_name);
		  defd_vars.emplace(var_name, "");
	    }
	    continue;
	      
	case 'E':
	    Eflag = 1;
	    break;

	case 'l':
	    lflag = 1;
	    break;

	case 'r':
	    rflag = 1;
	    break;

	case 't':
	    tflag = 1;
	    break;

	case 'v':
	    vflag = 1;
	    break;

	case 'S':
	    if (*++s)
		read_skel(s);
	    else if (++i < argc)
		read_skel(argv[i]);
	    else
		usage();
	    continue;

	default:
	    usage();
	}

	for (;;)
	{
	    switch (*++s)
	    {
	    case '\0':
		goto end_of_option;

	    case 'd':
		dflag = 1;
		break;

	    case 'l':
		lflag = 1;
		break;

	    case 'r':
		rflag = 1;
		break;

	    case 't':
		tflag = 1;
		break;

	    case 'v':
		vflag = 1;
		break;

	    default:
		usage();
	    }
	}
end_of_option:;
    }

no_more_options:;
    if (i + 1 != argc) usage();
    input_file_name = argv[i];

	if (file_prefix.empty()) {
		if (!input_file_name.empty()) {
			file_prefix = input_file_name.extension();
			if (file_prefix.empty()) file_prefix = "y";
		}
	}
}

char *allocate(unsigned n)
{
     char *p;

    p = NULL;
    if (n)
    {
        /* VM: add a few bytes here, cause 
         * Linux calloc does not like sizes like 32768 */
	p = (char*)CALLOC(1, n+10);
	if (!p) no_space();
    }
    return (p);
}


void create_file_names() // preferred_separator
{
    int i, len;

	action_file_name = text_file_name = union_file_name 
		= use_current_dir_as_temp ? std::filesystem::current_path() : std::filesystem::temp_directory_path();

	action_file_name /= temp_form;
	text_file_name /= temp_form;
	union_file_name /= temp_form;

	action_file_name += 'a';
	text_file_name += 't';
	union_file_name += 'u';

	// create temp files

	// make the file names
	output_file_name = file_prefix;
	output_file_name += OUTPUT_SUFFIX;

    if (rflag) {
		output_file_name = file_prefix;
		output_file_name += CODE_SUFFIX;
    } else
		code_file_name = output_file_name;

	if (dflag) {
		defines_file_name = file_prefix;
		defines_file_name += DEFINES_SUFFIX;
	}

	if (vflag) {
		verbose_file_name = file_prefix;
		verbose_file_name += VERBOSE_SUFFIX;
	}

}


void open_files()
{
    create_file_names();

	if (!input_file)
	{
		
		input_file = std::make_unique<std::ifstream>(input_file_name.c_str());
		if (input_file->bad())
			open_error(input_file_name.c_str());
	}

	action_file.open(action_file_name.c_str());
	if(action_file.bad()) open_error(action_file_name.c_str());
		
	text_file.open(text_file_name.c_str());
	if (action_file.bad()) open_error(text_file_name.c_str());


    if (vflag)
    {
		verbose_file.open(verbose_file_name.c_str());
		if (action_file.bad()) open_error(verbose_file_name.c_str());
    }

    if (dflag)
    {
		defines_file.open(verbose_file_name.c_str());
		if (action_file.bad()) open_error(defines_file_name.c_str());

		union_file.open(union_file_name.c_str());
		if (union_file.bad()) open_error(union_file_name.c_str());
    }

	output_file.open(output_file_name.c_str());
	if (output_file.bad()) open_error(output_file_name.c_str());


    if (rflag)
    {
		code_file.open(code_file_name.c_str());
		if (code_file.bad()) open_error(code_file_name.c_str());
    }
    else
	code_file = output_file;
}


int main(int argc, char **argv)
{
    set_signals();
    getargs(argc, argv);
    open_files();
    reader();
    lr0();
    lalr();
    make_parser();
    verbose();
    output();
    done(0);
    return 0;
}
