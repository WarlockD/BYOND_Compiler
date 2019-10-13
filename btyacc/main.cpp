#include "defs.h"
#include <signal.h>
#include <stdio.h>

#include <io.h>
#include <iostream>


char dflag;
char lflag;
char rflag;
char tflag;
char vflag;
int Eflag = 0;

std::filesystem::path file_prefix = "y";
std::filesystem::path myname = "yacc";
std::filesystem::path temp_form = "yacc_t_";/// XXXXXX";


std::filesystem::path action_file_name;
std::filesystem::path code_file_name;
std::filesystem::path defines_file_name;
std::filesystem::path input_file_name = "";
std::filesystem::path output_file_name;
std::filesystem::path text_file_name;
std::filesystem::path union_file_name;
std::filesystem::path verbose_file_name;

std::fstream action_file;	/*  a temp file, used to save actions associated    */
			/*  with rules until the parser is written	    */
std::fstream code_file;	/*  y.code.c (used when the -r option is specified) */
std::fstream defines_file;	/*  y.tab.h					    */
std::fstream input_file;	/*  the input file				    */
std::fstream output_file;	/*  y.tab.c					    */
std::fstream text_file;	/*  a temp file, used to save text until all	    */
			/*  symbols have been defined			    */
std::fstream union_file;	/*  a temp file, used to save the union		    */
			/*  definition until all symbol have been	    */
			/*  defined					    */
std::fstream verbose_file;	/*  y.output					    */


int lineno;
int outline;

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
std::vector<bool> nullable;

__declspec(noreturn)
void done(int k)
{
	if (action_file.is_open()) { action_file.close(); std::filesystem::remove(action_file_name);  }
    if (text_file.is_open()) { text_file.close(); std::filesystem::remove(text_file_name); }
	if (union_file.is_open()) { union_file.close(); std::filesystem::remove(union_file_name);}
    exit(k);
}

__declspec(noreturn)
void onintr(int i)
{
	//(void)i;
    done(1);
}


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


void usage()
{
    fprintf(stderr, "usage: %s [-dlrtv] [-b file_prefix] [-S skeleton file] "
		    "filename\n", myname.c_str());
    exit(1);
}


void getargs(int argc, const char ** argv)
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
		input_file.set_rdbuf(std::cin.rdbuf());
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
	    { char **ps;
	      char *var_name = s + 1;
	      extern char *defd_vars[];
	      for(ps=&defd_vars[0]; *ps; ps++) {
		if(strcmp(*ps,var_name)==0) {
		  error(lineno, 0, 0, "Preprocessor variable %s already defined", var_name);
		}
	      }
		  *ps = new char[strlen(var_name) + 1)];
		  strcpy(*ps, var_name);
	      *++ps = NULL;
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
			file_prefix = input_file_name;
			file_prefix = input_file_name.filename();
      } else {
	file_prefix = "y"; 
      }
    }
}



void create_file_names()
{
	std::filesystem::path tmpdir;
	for (size_t i = 0; i < 999999; i++) {
		tmpdir = std::filesystem::temp_directory_path();
		tmpdir /= temp_form;
		tmpdir += std::to_string(i);
		union_file_name = text_file_name = action_file_name = tmpdir;
		union_file_name += 'u';
		text_file_name += 't';
		action_file_name += 'a';

		// no way this fails?  Figure out a throw
		if (std::filesystem::exists(action_file_name) && !std::filesystem::remove(action_file_name)) continue;
		if (std::filesystem::exists(union_file_name) && !std::filesystem::remove(union_file_name)) continue;
		if (std::filesystem::exists(action_file_name) && !std::filesystem::remove(action_file_name)) continue;

	}

	output_file_name = file_prefix;
	output_file_name += OUTPUT_SUFFIX;

    if (rflag) {
		code_file_name = file_prefix;
		code_file_name += CODE_SUFFIX;
    }
    else
		code_file_name = output_file_name;

    if (dflag) {
		defines_file_name = file_prefix;
		defines_file_name += DEFINES_SUFFIX;
    }

    if (vflag){
		verbose_file_name = file_prefix;
		verbose_file_name += VERBOSE_SUFFIX;
    }
}

void open_file(std::fstream& fs, const std::filesystem::path& path, int mode) {
	fs.open(path, mode);
	if (!fs.is_open()) open_error(path.c_str());
}
void open_file_read(std::fstream& fs, const std::filesystem::path& path) { open_file(input_file, input_file_name, std::ios::in); }
void open_file_write(std::fstream& fs, const std::filesystem::path& path) { open_file(input_file, input_file_name, std::ios::out); }

void open_files()
{
    create_file_names();

	if (!input_file.is_open()) open_file_read(input_file, input_file_name);

	open_file_write(action_file, action_file_name);
	open_file_write(text_file, text_file_name);

    if (vflag) open_file_write(verbose_file, verbose_file_name);

	if (dflag) {
		open_file_write(defines_file, defines_file_name);
		open_file_write(union_file, union_file_name);
	}

	open_file_write(output_file, output_file_name);


	if (rflag) {
		open_file_write(code_file, code_file_name);
	}
	else
		code_file.set_rdbuf(output_file.rdbuf()); // 	code_file = output_file;
}


int main(int argc, const char ** argv)
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
