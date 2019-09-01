#include <filesystem>
#include <iostream>
#include <algorithm>
#include <variant>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <array>
#include <fstream>
#include <vector>
#include <queue>

using left_t = std::variant<int, char*>;
#define gint(V) std::get<int>(V)
#define gstr(V) std::get<char*>(V)

# define TOKENSIZE 1000
# define DEFSIZE 40
# define DEFCHAR 1000
# define STARTCHAR 100
# define STARTSIZE 256
# define CCLSIZE 1000
# define TREESIZE 1000
# define NSTATES 500
# define MAXPOS 2500
# define NTRANS 2000
# define NOUTPUT 3000


# define NACTIONS 100
# define ALITTLEEXTRA 30

# define RCCL NCH+90
# define RNCCL NCH+91
# define RSTR NCH+92
# define RSCON NCH+93
# define RNEWE NCH+94
# define FINAL NCH+95
# define RNULLS NCH+96
# define RCAT NCH+97
# define STAR NCH+98
# define PLUS NCH+99
# define QUEST NCH+100
# define DIV NCH+101
# define BAR NCH+102
# define CARAT NCH+103
# define S1FINAL NCH+104
# define S2FINAL NCH+105

# define DEFSECTION 1
# define RULESECTION 2
# define ENDSECTION 5
# define TRUE 1
# define FALSE 0

# define PC 1
# define PS 1

#define CWIDTH 7
#define CMASK 0177
#define ASCII 1
#define NCH 128

#ifdef DEBUG
# define LINESIZE 110
int yydebug;
int debug;		/* 1 = on */
int charc;
#endif

#ifndef DEBUG
#define freturn(s) s
#endif
#if 0
% token CHAR CCL NCCL STR DELIM SCON ITER NEWE NULLS
% left SCON '/' NEWE
% left '|'
% left '$' '^'
% left CHAR CCL NCCL '(' '.' STR NULLS
% left ITER
% left CAT
% left '*' '+' '?'
#endif
enum { 
	CHAR = 1000,
	CCL, NCCL, STR, DELIM, SCON, ITER, NEWE, NULLS, CAT

};



int ccount = 1;
int casecount = 1;
int aptr = 1;
int nstates = NSTATES, maxpos = MAXPOS;
int treesize = TREESIZE, ntrans = NTRANS;
int yytop;
int outsize = NOUTPUT;
int sptr = 1;
int optim = TRUE;
int report = 2;
int debug;		/* 1 = on */
int charc;
int sargc;
const char **sargv;
char buf[520];
int yyline;		/* line number of file */
int eof;
int lgatflg;
int divflg;
int funcflag;
int pflag;
int chset;	/* 1 = char set modified */
std::ifstream* fin=nullptr, * fother = nullptr;
std::queue<std::filesystem::path> file_list;
int *name;

left_t* left;
int *right;
int *parent;
char *nullstr;
int tptr;

char slist[STARTSIZE];
char **def, **subs, *dchar;
char **sname, *schar;
char *ccl;
char *ccptr;
char *dp, *sp;
int dptr;
char *bptr;		/* store input position */
char *tmpstat;
int count;
int **foll;
int *nxtpos;
int *positions;
int *gotof;
int *nexts;
char *nchar;
int **state;
int *sfall;		/* fallback state num */
char *cpackflg;		/* true if state has been character packed */
int *atable;
int nptr;
char symbol[NCH];
char cindex[NCH];
int xstate;
int stnum;
char match[NCH];
char extra[NACTIONS];
char *pchar, *pcptr;
int pchlen = TOKENSIZE;
 long rcount;
int *verify, *advance, *stoff;
int scon;
char *psave;
int buserr(), segviol();

int	ZCH = NCH;
FILE* fout = NULL, * errorf = { stdout };
int	sect = DEFSECTION;
int	prev = '\n';	/* previous input character */
int	pres = '\n';	/* present input character */
int	peek = '\n';	/* next input character */
//char pushc[TOKENSIZE];
//char* pushptr = pushc;
std::queue<char> pushc;
char* slptr = slist;


int	ctable[2 * NCH] = {
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
100,101,102,103,104,105,106,107,108,109,
110,111,112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127 };

void phead1() {
	fprintf(fout, "# include \"stdio.h\"\n");
	if (ZCH > NCH)
		fprintf(fout, "# define U(x) ((x)&0377)\n");
	else
		fprintf(fout, "# define U(x) x\n");
	fprintf(fout, "# define NLSTATE yyprevious=YYNEWLINE\n");
	fprintf(fout, "# define BEGIN yybgin = yysvec + 1 +\n");
	fprintf(fout, "# define INITIAL 0\n");
	fprintf(fout, "# define YYLERR yysvec\n");
	fprintf(fout, "# define YYSTATE (yyestate-yysvec-1)\n");
	if (optim)
		fprintf(fout, "# define YYOPTIM 1\n");
# ifdef DEBUG
	fprintf(fout, "# define LEXDEBUG 1\n");
# endif
	fprintf(fout, "# define YYLMAX 200\n");
	fprintf(fout, "# define output(c) putc(c,yyout)\n");
	fprintf(fout, "%s%d%s\n",
		"# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==",
		ctable['\n'],
		"?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)");
	fprintf(fout,
		"# define unput(c) {yytchar= (c);if(yytchar=='\\n')yylineno--;*yysptr++=yytchar;}\n");
	fprintf(fout, "# define yymore() (yymorfg=1)\n");
	fprintf(fout, "# define ECHO fprintf(yyout, \"%%s\",yytext)\n");
	fprintf(fout, "# define REJECT { nstr = yyreject(); goto yyfussy;}\n");
	fprintf(fout, "int yyleng; extern char yytext[];\n");
	fprintf(fout, "int yymorfg;\n");
	fprintf(fout, "extern char *yysptr, yysbuf[];\n");
	fprintf(fout, "int yytchar;\n");
	fprintf(fout, "FILE *yyin = {stdin}, *yyout = {stdout};\n");
	fprintf(fout, "extern int yylineno;\n");
	fprintf(fout, "struct yysvf { \n");
	fprintf(fout, "\tstruct yywork *yystoff;\n");
	fprintf(fout, "\tstruct yysvf *yyother;\n");
	fprintf(fout, "\tint *yystops;};\n");
	fprintf(fout, "struct yysvf *yyestate;\n");
	fprintf(fout, "extern struct yysvf yysvec[], *yybgin;\n");
}

void phead2() {
	fprintf(fout, "while((nstr = yylook()) >= 0)\n");
	fprintf(fout, "yyfussy: switch(nstr){\n");
	fprintf(fout, "case 0:\n");
	fprintf(fout, "if(yywrap()) return(0); break;\n");
}

void ptail() {
	if (!pflag) {
		fprintf(fout, "case -1:\nbreak;\n");		/* for reject */
		fprintf(fout, "default:\n");
		fprintf(fout, "fprintf(yyout,\"bad switch yylook %%d\",nstr);\n");
		fprintf(fout, "} return(0); }\n");
		fprintf(fout, "/* end of yylex */\n");
	}

	pflag = 1;
}

void statistics() {
	fprintf(errorf, "%d/%d nodes(%%e), %d/%d positions(%%p), %d/%d (%%n), %ld transitions\n",
		tptr, treesize, nxtpos - positions, maxpos, stnum + 1, nstates, rcount);
	fprintf(errorf, ", %d/%d packed char classes(%%k)", pcptr - pchar, pchlen);
	if (optim)fprintf(errorf, ", %d/%d packed transitions(%%a)", nptr, ntrans);
	fprintf(errorf, ", %d/%d output slots(%%o)", yytop, outsize);
	putc('\n', errorf);
}

template<typename ... ARGS>
void error(const char* s, ARGS ... args)
{
	if (!eof)fprintf(errorf, "%d: ", yyline);
	fprintf(errorf, "(Error) ");
	fprintf(errorf, s, args...);
	putc('\n', errorf);
# ifdef DEBUG
	if (debug && sect != ENDSECTION) {
		sect1dump();
		sect2dump();
	}
# endif
	if (
# ifdef DEBUG
		debug ||
# endif
		report == 1) statistics();
	exit(1);	/* error return code */
}
template<typename ... ARGS>
void warning(const char* s, ARGS ... args)
{
	if (!eof)fprintf(errorf, "%d: ", yyline);
	fprintf(errorf, "(Warning) ");
	fprintf(errorf, s, args...);
	putc('\n', errorf);
	fflush(errorf);
	fflush(fout);
	fflush(stdout);
}

void cfree(void* ptr, int elements, int size) {
	(void)elements, size;
	free(ptr);

}
char* myalloc(int a, int b) {
	void* i = calloc(a, b);
	if (i == 0)
		warning("OOPS - calloc returns a 0");
	else if (i == (void*)-1) {
# ifdef DEBUG
		warning("calloc returns a -1");
# endif
		return NULL;
	}
	return (char*)i;
}
void get1core();
void free1core();
void get2core();
void free2core();
void get3core();
void packtrans(int st, char* tch, int* tst, int cnt, int tryit);
int gch();
int slength(char*);
int notin(int n);


char* getl(char* p)	/* return next line of input, throw away trailing '\n' */
	/* returns 0 if eof is had immediately */
{
	 int c;
	 char* s, * t;
	t = s = p;
	while (((c = gch()) != 0) && c != '\n')
		* t++ = c;
	*t = 0;
	if (c == 0 && s == t) return((char*)0);
	prev = '\n';
	pres = '\n';
	return(s);
}
bool space(char ch)
{
	switch (ch)
	{
	case ' ':
	case '\t':
	case '\n':
		return true;
	}
	return false;
}

bool digit(char c)
{
	return(c >= '0' && c <= '9');
}



bool alpha(char c) { return('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z'); }

bool printable(char c) { return(c > 040 && c < 0177); }

void lgate()
{
	char fname[20];
	if (lgatflg) return;
	lgatflg = 1;
	if (fout == NULL) {
		sprintf(fname, "lex.yy.%c",  'c');
		fout = fopen(fname, "w"); 
	}
	if (fout == NULL) error("Can't open %s", fname);
	phead1();
}
/* scopy(ptr to str, ptr to str) - copy first arg str to second */
/* returns ptr to second arg */
void dscopy(const char* s, char* t){
	char* i;
	i = t;
	while (*i++ = *s++);
}
int siconv(char* t){	/* convert string t, return integer value */

	int i, sw;
	char* s;
	s = t;
	while (!(('0' <= *s && *s <= '9') || *s == '-') && *s) s++;
	sw = 0;
	if (*s == '-') {	/* neg */
		sw = 1;
		s++;
	}
	i = 0;
	while ('0' <= *s && *s <= '9')
		i = i * 10 + (*(s++) - '0');
	return(sw ? -i : i);
}
/* slength(ptr to str) - return integer length of string arg */
/* excludes '\0' terminator */
int slength(char* s) {
	int n;
	char* t;
	t = s;
	for (n = 0; *t++; n++);
	return(n);
}
/* strcmp(x,y) - return -1 if x < y,
		0 if x == y,
		return 1 if x > y, all lexicographically */

int ctrans(char** ss)
{
	int c, k;
	if ((c = **ss) != '\\')
		return(c);
	switch (c = *++ * ss)
	{
	case 'n': c = '\n'; break;
	case 't': c = '\t'; break;
	case 'r': c = '\r'; break;
	case 'b': c = '\b'; break;
	case 'f': c = 014; break;		/* form feed for ascii */
	case '\\': c = '\\'; break;
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		c -= '0';
		while ((k = *(*ss + 1)) >= '0' && k <= '7')
		{
			c = c * 8 + k - '0';
			(*ss)++;
		}
		break;
	}
	return(c);
}
void cclinter(int sw){
	/* sw = 1 ==> ccl */
	 int i, j, k;
	int m;
	if (!sw) {		/* is NCCL */
		for (i = 1; i < NCH; i++)
			symbol[i] ^= 1;			/* reverse value */
	}
	for (i = 1; i < NCH; i++)
		if (symbol[i]) break;
	if (i >= NCH) return;
	i = cindex[i];
	/* see if ccl is already in our table */
	j = 0;
	if (i) {
		for (j = 1; j < NCH; j++) {
			if ((symbol[j] && cindex[j] != i) ||
				(!symbol[j] && cindex[j] == i)) break;
		}
	}
	if (j >= NCH) return;		/* already in */
	m = 0;
	k = 0;
	for (i = 1; i < NCH; i++)
		if (symbol[i]) {
			if (!cindex[i]) {
				cindex[i] = ccount;
				symbol[i] = 0;
				m = 1;
			}
			else k = 1;
		}
	/* m == 1 implies last value of ccount has been used */
	if (m)ccount++;
	if (k == 0) return;	/* is now in as ccount wholly */
	/* intersection must be computed */
	for (i = 1; i < NCH; i++) {
		if (symbol[i]) {
			m = 0;
			j = cindex[i];	/* will be non-zero */
			for (k = 1; k < NCH; k++) {
				if (cindex[k] == j) {
					if (symbol[k]) symbol[k] = 0;
					else {
						cindex[k] = ccount;
						m = 1;
					}
				}
			}
			if (m)ccount++;
		}
	}
	return;
}
int usescape(int c) {
	 char d;
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
int lookup(char* s, char** t){
	 int i;
	i = 0;
	while (*t) {
		if (strcmp(s, *t) == 0)
			return(i);
		i++;
		t++;
	}
	return(-1);
}
int cpyact() { /* copy C action to the next ; or closing } */
	 int brac, c, mth;
	int savline, sw;

	brac = 0;
	sw = TRUE;

	while (!eof) {
		c = gch();
	swt:
		switch (c) {

		case '|':	if (brac == 0 && sw == TRUE) {
			if (peek == '|')gch();		/* eat up an extra '|' */
			return(0);
		}
					break;

		case ';':
			if (brac == 0) {
				putc(c, fout);
				putc('\n', fout);
				return(1);
			}
			break;

		case '{':
			brac++;
			savline = yyline;
			break;

		case '}':
			brac--;
			if (brac == 0) {
				putc(c, fout);
				putc('\n', fout);
				return(1);
			}
			break;

		case '/':	/* look for comments */
			putc(c, fout);
			c = gch();
			if (c != '*') goto swt;

			/* it really is a comment */

			putc(c, fout);
			savline = yyline;
			while (c = gch()) {
				if (c == '*') {
					putc(c, fout);
					if ((c = gch()) == '/') goto loop;
				}
				putc(c, fout);
			}
			yyline = savline;
			error("EOF inside comment");

		case '\'':	/* character constant */
			mth = '\'';
			goto string;

		case '"':	/* character string */
			mth = '"';

		string:

			putc(c, fout);
			while (c = gch()) {
				if (c == '\\') {
					putc(c, fout);
					c = gch();
				}
				else if (c == mth) goto loop;
				putc(c, fout);
				if (c == '\n')
				{
					yyline--;
					error("Non-terminated string or character constant");
				}
			}
			error("EOF in string or character constant");

		case '\0':
			yyline = savline;
			error("Action does not terminate");
		default:
			break;		/* usual character */
		}
	loop:
		if (c != ' ' && c != '\t' && c != '\n') sw = FALSE;
		putc(c, fout);
	}
	error("Premature EOF");
	return -1;
}
int gch() {
	int c = EOF;
		while (fin == nullptr || fin->eof()) {
			if (file_list.empty()) {
				if (fin) delete fin;
				return 0; // no files, or end of files
			}
			if (fin) delete fin;
			fin = new std::ifstream(file_list.front());
			if (!fin || fin->bad()) {
				error("Cannot open file %s", file_list.front().c_str());
			}
			//prev = pres = ' '; // just space filler
		//	peek = fin->get();
		};
	prev = pres;
	c = pres = peek;
	if (!pushc.empty()) {
		peek = pushc.front();
		pushc.pop();
	}
	else
		peek = fin->get();

	if (c == '\n')yyline++;
	return(c);
}

int mn2(int a, int d, int c) {
	name[tptr] = a;
	left[tptr] = d;
	right[tptr] = c;
	parent[tptr] = 0;
	nullstr[tptr] = 0;
	switch (a) {
	case RSTR:
		parent[d] = tptr;
		break;
	case BAR:
	case RNEWE:
		if (nullstr[d] || nullstr[c]) nullstr[tptr] = TRUE;
		parent[d] = parent[c] = tptr;
		break;
	case RCAT:
	case DIV:
		if (nullstr[d] && nullstr[c])nullstr[tptr] = TRUE;
		parent[d] = parent[c] = tptr;
		break;
	case RSCON:
		parent[d] = tptr;
		nullstr[tptr] = nullstr[d];
		break;
# ifdef DEBUG
	default:
		warning("bad switch mn2 %d %d", a, d);
		break;
# endif
	}
	if (tptr > treesize)
		error("Parse tree too big %s", (treesize == TREESIZE ? "\nTry using %e num" : ""));
	return(tptr++);
}

int mn1(int a, left_t d)
{
	name[tptr] = a;
	left[tptr] = d;
	parent[tptr] = 0;
	nullstr[tptr] = 0;
	switch (a) {
	case RCCL:
	case RNCCL:
		if (strlen(gstr(d)) == 0) nullstr[tptr] = TRUE;
		break;
	case STAR:
	case QUEST:
		nullstr[tptr] = TRUE;
		parent[gint(d)] = tptr;
		break;
	case PLUS:
	case CARAT:
		nullstr[tptr] = nullstr[gint(d)];
		parent[gint(d)] = tptr;
		break;
	case S2FINAL:
		nullstr[tptr] = TRUE;
		break;
# ifdef DEBUG
	case FINAL:
	case S1FINAL:
		break;
	default:
		warning("bad switch mn1 %d %d", a, d);
		break;
# endif
	}
	if (tptr > treesize)
		error("Parse tree too big %s", (treesize == TREESIZE ? "\nTry using %e num" : ""));
	return(tptr++);
}
int mn0(int a)
{
	name[tptr] = a;
	parent[tptr] = 0;
	nullstr[tptr] = 0;
	if (a >= NCH) switch (a) {
	case RNULLS: nullstr[tptr] = TRUE; break;
# ifdef DEBUG
	default:
		warning("bad switch mn0 %d", a);
		break;
# endif
	}
	if (tptr > treesize)
		error("Parse tree too big %s", (treesize == TREESIZE ? "\nTry using %e num" : ""));
	return(tptr++);
}
void _post_munput() {
# ifdef DEBUG
	else error("Unrecognized munput option %c", t);
# endif
	if (pushc.size() > TOKENSIZE)
		error("Too many characters pushed");
}
void munput(int p) {
	pushc.push(peek);		/* watch out for this */
	peek = (int)p;
	_post_munput();
}
void munput(const char* p) {
	int i, j;
	pushc.push(peek);
	peek = p[0];
	i = strlen(p);
	for (j = i - 1; j >= 1; j--)
		pushc.push(p[j]);
	_post_munput();
}
template<typename P>
void munput(int t, P p)	{/* implementation dependent */
	if (t == 'c') {
		munput((int)p);
	}
	else if (t == 's') {
		munput((const char*)p);
	}
}


int dupl(int n){
	/* duplicate the subtree whose root is n, return ptr to it */
	int i;
	i = name[n];
	if (i < NCH) return(mn0(i));
	switch (i) {
	case RNULLS:
		return(mn0(i));
	case RCCL: case RNCCL: case FINAL: case S1FINAL: case S2FINAL:
		return(mn1(i, left[n]));
	case STAR: case QUEST: case PLUS: case CARAT:
		return(mn1(i, dupl(gint(left[n]))));
	case RSTR: case RSCON:
		return(mn2(i, dupl(gint(left[n])), right[n]));
	case BAR: case RNEWE: case RCAT: case DIV:
		return(mn2(i, dupl(gint(left[n])), dupl(right[n])));
# ifdef DEBUG
	default:
		warning("bad switch dupl %d", n);
# endif
	}
	return(0);
}
# ifdef DEBUG
void allprint(char c) {
	switch (c) {
	case 014:
		printf("\\f");
		charc++;
		break;
	case '\n':
		printf("\\n");
		charc++;
		break;
	case '\t':
		printf("\\t");
		charc++;
		break;
	case '\b':
		printf("\\b");
		charc++;
		break;
	case ' ':
		printf("\\\bb");
		break;
	default:
		if (!printable(c)) {
			printf("\\%-3o", c);
			charc = +3;
		}
		else
			putchar(c);
		break;
	}
	charc++;
}
void strpt(char* s)
char* s; {
	charc = 0;
	while (*s) {
		allprint(*s++);
		if (charc > LINESIZE) {
			charc = 0;
			printf("\n\t");
		}
	}
}
void sect1dump() {
	 int i;
	printf("Sect 1:\n");
	if (def[0]) {
		printf("str	trans\n");
		i = -1;
		while (def[++i])
			printf("%s\t%s\n", def[i], subs[i]);
	}
	if (sname[0]) {
		printf("start names\n");
		i = -1;
		while (sname[++i])
			printf("%s\n", sname[i]);
	}
	if (chset == TRUE) {
		printf("char set changed\n");
		for (i = 1; i < NCH; i++) {
			if (i != ctable[i]) {
				allprint(i);
				putchar(' ');
				printable(ctable[i]) ? putchar(ctable[i]) : printf("%d", ctable[i]);
				putchar('\n');
			}
		}
	}
}
void sect2dump() {
	printf("Sect 2:\n");
	treedump();
}
void treedump()
{
	 int t;
	 char* p;
	printf("treedump %d nodes:\n", tptr);
	for (t = 0; t < tptr; t++) {
		printf("%4d ", t);
		parent[t] ? printf("p=%4d", parent[t]) : printf("      ");
		printf("  ");
		if (name[t] < NCH) {
			allprint(name[t]);
		}
		else switch (name[t]) {
		case RSTR:
			printf("%d ", left[t]);
			allprint(right[t]);
			break;
		case RCCL:
			printf("ccl ");
			strpt(left[t]);
			break;
		case RNCCL:
			printf("nccl ");
			strpt(left[t]);
			break;
		case DIV:
			printf("/ %d %d", left[t], right[t]);
			break;
		case BAR:
			printf("| %d %d", left[t], right[t]);
			break;
		case RCAT:
			printf("cat %d %d", left[t], right[t]);
			break;
		case PLUS:
			printf("+ %d", left[t]);
			break;
		case STAR:
			printf("* %d", left[t]);
			break;
		case CARAT:
			printf("^ %d", left[t]);
			break;
		case QUEST:
			printf("? %d", left[t]);
			break;
		case RNULLS:
			printf("nullstring");
			break;
		case FINAL:
			printf("final %d", left[t]);
			break;
		case S1FINAL:
			printf("s1final %d", left[t]);
			break;
		case S2FINAL:
			printf("s2final %d", left[t]);
			break;
		case RNEWE:
			printf("new %d %d", left[t], right[t]);
			break;
		case RSCON:
			p = right[t];
			printf("start %s", sname[*p++ - 1]);
			while (*p)
				printf(", %s", sname[*p++ - 1]);
			printf(" %d", left[t]);
			break;
		default:
			printf("unknown %d %d %d", name[t], left[t], right[t]);
			break;
		}
		if (nullstr[t])printf("\t(null poss.)");
		putchar('\n');
	}
}
# endif
void first(int);
void acompute(int);
void nextstate(int, int);
void cgoto();
void add(int**, int);
void follow(int);
int member(int d, char* t);

void cfoll(int v)
{
	int i, j, k;
	char* p;
	i = name[v];
	if (i < NCH) i = 1;	/* character */
	switch (i) {
	case 1: case RSTR: case RCCL: case RNCCL: case RNULLS:
		for (j = 0; j < tptr; j++)
			tmpstat[j] = FALSE;
		count = 0;
		follow(v);
# ifdef PP
		padd(foll, v);		/* packing version */
# endif
# ifndef PP
		add(foll, v);		/* no packing version */
# endif
		if (i == RSTR) cfoll(gint(left[v]));
		else if (i == RCCL || i == RNCCL) {	/* compress ccl list */
			for (j = 1; j < NCH; j++)
				symbol[j] = (i == RNCCL);
			p = gstr(left[v]);
			while (*p)
				symbol[*p++] = (i == RCCL);
			p = pcptr;
			for (j = 1; j < NCH; j++)
				if (symbol[j]) {
					for (k = 0; p + k < pcptr; k++)
						if (cindex[j] == *(p + k))
							break;
					if (p + k >= pcptr)* pcptr++ = cindex[j];
				}
			*pcptr++ = 0;
			if (pcptr > pchar + pchlen)
				error("Too many packed character classes");
			left[v] = (int)p;
			name[v] = RCCL;	/* RNCCL eliminated */
# ifdef DEBUG
			if (debug && *p) {
				printf("ccl %d: %d", v, *p++);
				while (*p)
					printf(", %d", *p++);
				putchar('\n');
			}
# endif
		}
		break;
	case CARAT:
		cfoll(gint(left[v]));
		break;
	case STAR: case PLUS: case QUEST: case RSCON:
		cfoll(gint(left[v]));
		break;
	case BAR: case RCAT: case DIV: case RNEWE:
		cfoll(gint(left[v]));
		cfoll(right[v]);
		break;
# ifdef DEBUG
	case FINAL:
	case S1FINAL:
	case S2FINAL:
		break;
	default:
		warning("bad switch cfoll %d", v);
# endif
	}
}
# ifdef DEBUG
pfoll()
{
	 int i, k, * p;
	int j;
	/* print sets of chars which may follow positions */
	printf("pos\tchars\n");
	for (i = 0; i < tptr; i++)
		if (p = foll[i]) {
			j = *p++;
			if (j >= 1) {
				printf("%d:\t%d", i, *p++);
				for (k = 2; k <= j; k++)
					printf(", %d", *p++);
				putchar('\n');
			}
		}
	return;
}
# endif

void add(int** array, int n){
	int i, * temp;
	char* ctemp;
	temp = nxtpos;
	ctemp = tmpstat;
	array[n] = nxtpos;		/* note no packing is done in positions */
	*temp++ = count;
	for (i = 0; i < tptr; i++)
		if (ctemp[i] == TRUE)
			* temp++ = i;
	nxtpos = temp;
	if (nxtpos >= positions + maxpos)
		error("Too many positions %s", (maxpos == MAXPOS ? "\nTry using %p num" : ""));
	return;
}

void follow(int v)
{
	 int p;
	if (v >= tptr - 1)return;
	p = parent[v];
	if (p == 0) return;
	switch (name[p]) {
		/* will not be CHAR RNULLS FINAL S1FINAL S2FINAL RCCL RNCCL */
	case RSTR:
		if (tmpstat[p] == FALSE) {
			count++;
			tmpstat[p] = TRUE;
		}
		break;
	case STAR: case PLUS:
		first(v);
		follow(p);
		break;
	case BAR: case QUEST: case RNEWE:
		follow(p);
		break;
	case RCAT: case DIV:
		if (v == gint(left[p])) {
			if (nullstr[right[p]])
				follow(p);
			first(right[p]);
		}
		else follow(p);
		break;
	case RSCON: case CARAT:
		follow(p);
		break;
# ifdef DEBUG
	default:
		warning("bad switch follow %d", p);
# endif
	}
	return;
}

void first(int v)	{/* calculate set of positions with v as root which can be active initially */
	int i;
	char* p;
	i = name[v];
	if (i < NCH)i = 1;
	switch (i) {
	case 1: case RCCL: case RNCCL: case RNULLS: case FINAL: case S1FINAL: case S2FINAL:
		if (tmpstat[v] == FALSE) {
			count++;
			tmpstat[v] = TRUE;
		}
		break;
	case BAR: case RNEWE:
		first(gint(left[v]));
		first(right[v]);
		break;
	case CARAT:
		if (stnum % 2 == 1)
			first(gint(left[v]));
		break;
	case RSCON:
		i = stnum / 2 + 1;
		p = (char*)right[v];
		while (*p)
			if (*p++ == i) {
				first(gint(left[v]));
				break;
			}
		break;
	case STAR: case QUEST: case PLUS:  case RSTR:
		first(gint(left[v]));
		break;
	case RCAT: case DIV:
		first(gint(left[v]));
		if (nullstr[gint(left[v])])
			first(right[v]);
		break;
# ifdef DEBUG
	default:
		warning("bad switch first %d", v);
# endif
	}
}

void cgoto() {
	 int i, j, s;
	int npos, curpos, n;
	int tryit;
	char tch[NCH];
	int tst[NCH];
	char* q;
	/* generate initial state, for each start condition */
	fprintf(fout, "int yyvstop[] = {\n0,\n");
	while (stnum < 2 || stnum / 2 < sptr) {
		for (i = 0; i < tptr; i++) tmpstat[i] = 0;
		count = 0;
		if (tptr > 0)first(tptr - 1);
		add(state, stnum);
# ifdef DEBUG
		if (debug) {
			if (stnum > 1)
				printf("%s:\n", sname[stnum / 2]);
			pstate(stnum);
		}
# endif
		stnum++;
	}
	stnum--;
	/* even stnum = might not be at line begin */
	/* odd stnum  = must be at line begin */
	/* even states can occur anywhere, odd states only at line begin */
	for (s = 0; s <= stnum; s++) {
		tryit = FALSE;
		cpackflg[s] = FALSE;
		sfall[s] = -1;
		acompute(s);
		for (i = 0; i < NCH; i++) symbol[i] = 0;
		npos = *state[s];
		for (i = 1; i <= npos; i++) {
			curpos = *(state[s] + i);
			if (name[curpos] < NCH) symbol[name[curpos]] = TRUE;
			else switch (name[curpos]) {
			case RCCL:
				tryit = TRUE;
				q = gstr(left[curpos]);
				while (*q) {
					for (j = 1; j < NCH; j++)
						if (cindex[j] == *q)
							symbol[j] = TRUE;
					q++;
				}
				break;
			case RSTR:
				symbol[right[curpos]] = TRUE;
				break;
# ifdef DEBUG
			case RNULLS:
			case FINAL:
			case S1FINAL:
			case S2FINAL:
				break;
			default:
				warning("bad switch cgoto %d state %d", curpos, s);
				break;
# endif
			}
		}
# ifdef DEBUG
		if (debug) {
			printf("State %d transitions on:\n\t", s);
			charc = 0;
			for (i = 1; i < NCH; i++) {
				if (symbol[i]) allprint(i);
				if (charc > LINESIZE) {
					charc = 0;
					printf("\n\t");
				}
			}
			putchar('\n');
		}
# endif
		/* for each char, calculate next state */
		n = 0;
		for (i = 1; i < NCH; i++) {
			if (symbol[i]) {
				nextstate(s, i);		/* executed for each state, transition pair */
				xstate = notin(stnum);
				if (xstate == -2) warning("bad state  %d %o", s, i);
				else if (xstate == -1) {
					if (stnum >= nstates)
						error("Too many states %s", (nstates == NSTATES ? "\nTry using %n num" : ""));
					add(state, ++stnum);
# ifdef DEBUG
					if (debug)pstate(stnum);
# endif
					tch[n] = i;
					tst[n++] = stnum;
				}
				else {		/* xstate >= 0 ==> state exists */
					tch[n] = i;
					tst[n++] = xstate;
				}
			}
		}
		tch[n] = 0;
		tst[n] = -1;
		/* pack transitions into permanent array */
		if (n > 0) packtrans(s, tch, tst, n, tryit);
		else gotof[s] = -1;
	}
	 fprintf(fout, "0};\n");
	return;
}
/*	Beware -- 70% of total CPU time is spent in this subroutine -
	if you don't believe me - try it yourself ! */
void nextstate(int s, int c){
	 int j, * newpos;
	 char* temp, * tz;
	int* pos, i, * f, num, curpos, number;
	/* state to goto from state s on char c */
	num = *state[s];
	temp = tmpstat;
	pos = state[s] + 1;
	for (i = 0; i < num; i++) {
		curpos = *pos++;
		j = name[curpos];
		if (j < NCH && j == c
			|| j == RSTR && c == right[curpos]
			|| j == RCCL && member(c, std::get<char*>(left[curpos]))) {
			f = foll[curpos];
			number = *f;
			newpos = f + 1;
			for (j = 0; j < number; j++)
				temp[*newpos++] = 2;
		}
	}
	j = 0;
	tz = temp + tptr;
	while (temp < tz) {
		if (*temp == 2) {
			j++;
			*temp++ = 1;
		}
		else *temp++ = 0;
	}
	count = j;
}
int notin(int n) {	/* see if tmpstat occurs previously */
	 int* j, k;
	 char* temp;
	int i;
	if (count == 0)
		return(-2);
	temp = tmpstat;
	for (i = n; i >= 0; i--) {	/* for each state */
		j = state[i];
		if (count == *j++) {
			for (k = 0; k < count; k++)
				if (!temp[*j++])break;
			if (k >= count)
				return(i);
		}
	}
	return(-1);
}
void packtrans(int st,char* tch,int*tst, int cnt,int tryit) {
	/* pack transitions into nchar, nexts */
	/* nchar is terminated by '\0', nexts uses cnt, followed by elements */
	/* gotof[st] = index into nchr, nexts for state st */

	/* sfall[st] =  t implies t is fall back state for st */
	/*	        == -1 implies no fall back */

	int cmin, cval, tcnt, diff, p, * ast;
	 int i, j, k;
	char* ach;
	int go[NCH], temp[NCH], c;
	int swork[NCH];
	char cwork[NCH];
	int upper;

	rcount += cnt;
	cmin = -1;
	cval = NCH;
	ast = tst;
	ach = tch;
	/* try to pack transitions using ccl's */
	if (!optim)goto nopack;		/* skip all compaction */
	if (tryit) {	/* ccl's used */
		for (i = 1; i < NCH; i++) {
			go[i] = temp[i] = -1;
			symbol[i] = 1;
		}
		for (i = 0; i < cnt; i++) {
			go[tch[i]] = tst[i];
			symbol[tch[i]] = 0;
		}
		for (i = 0; i < cnt; i++) {
			c = match[tch[i]];
			if (go[c] != tst[i] || c == tch[i])
				temp[tch[i]] = tst[i];
		}
		/* fill in error entries */
		for (i = 1; i < NCH; i++)
			if (symbol[i]) temp[i] = -2;	/* error trans */
		/* count them */
		k = 0;
		for (i = 1; i < NCH; i++)
			if (temp[i] != -1)k++;
		if (k < cnt) {	/* compress by char */
# ifdef DEBUG
			if (debug) printf("use compression  %d,  %d vs %d\n", st, k, cnt);
# endif
			k = 0;
			for (i = 1; i < NCH; i++)
				if (temp[i] != -1) {
					cwork[k] = i;
					swork[k++] = (temp[i] == -2 ? -1 : temp[i]);
				}
			cwork[k] = 0;
# ifdef PC
			ach = cwork;
			ast = swork;
			cnt = k;
			cpackflg[st] = TRUE;
# endif
		}
	}
	for (i = 0; i < st; i++) {	/* get most similar state */
				/* reject state with more transitions, state already represented by a third state,
					and state which is compressed by char if ours is not to be */
		if (sfall[i] != -1) continue;
		if (cpackflg[st] == 1) if (!(cpackflg[i] == 1)) continue;
		p = gotof[i];
		if (p == -1) /* no transitions */ continue;
		tcnt = nexts[p];
		if (tcnt > cnt) continue;
		diff = 0;
		k = 0;
		j = 0;
		upper = p + tcnt;
		while (ach[j] && p < upper) {
			while (ach[j] < nchar[p] && ach[j]) { diff++; j++; }
			if (ach[j] == 0)break;
			if (ach[j] > nchar[p]) { diff = NCH; break; }
			/* ach[j] == nchar[p] */
			if (ast[j] != nexts[++p] || ast[j] == -1 || (cpackflg[st] && ach[j] != match[ach[j]]))diff++;
			j++;
		}
		while (ach[j]) {
			diff++;
			j++;
		}
		if (p < upper)diff = NCH;
		if (diff < cval && diff < tcnt) {
			cval = diff;
			cmin = i;
			if (cval == 0)break;
		}
	}
	/* cmin = state "most like" state st */
# ifdef DEBUG
	if (debug)printf("select st %d for st %d diff %d\n", cmin, st, cval);
# endif
# ifdef PS
	if (cmin != -1) { /* if we can use st cmin */
		gotof[st] = nptr;
		k = 0;
		sfall[st] = cmin;
		p = gotof[cmin] + 1;
		j = 0;
		while (ach[j]) {
			/* if cmin has a transition on c, then so will st */
			/* st may be "larger" than cmin, however */
			while (ach[j] < nchar[p - 1] && ach[j]) {
				k++;
				nchar[nptr] = ach[j];
				nexts[++nptr] = ast[j];
				j++;
			}
			if (nchar[p - 1] == 0)break;
			if (ach[j] > nchar[p - 1]) {
				warning("bad transition %d %d", st, cmin);
				goto nopack;
			}
			/* ach[j] == nchar[p-1] */
			if (ast[j] != nexts[p] || ast[j] == -1 || (cpackflg[st] && ach[j] != match[ach[j]])) {
				k++;
				nchar[nptr] = ach[j];
				nexts[++nptr] = ast[j];
			}
			p++;
			j++;
		}
		while (ach[j]) {
			nchar[nptr] = ach[j];
			nexts[++nptr] = ast[j++];
			k++;
		}
		nexts[gotof[st]] = cnt = k;
		nchar[nptr++] = 0;
	}
	else {
# endif
	nopack:
		/* stick it in */
		gotof[st] = nptr;
		nexts[nptr] = cnt;
		for (i = 0; i < cnt; i++) {
			nchar[nptr] = ach[i];
			nexts[++nptr] = ast[i];
		}
		nchar[nptr++] = 0;
# ifdef PS
	}
# endif
	if (cnt < 1) {
		gotof[st] = -1;
		nptr--;
	}
	else
		if (nptr > ntrans)
			error("Too many transitions %s", (ntrans == NTRANS ? "\nTry using %a num" : ""));
}
# ifdef DEBUG
pstate(s)
int s; {
	 int* p, i, j;
	printf("State %d:\n", s);
	p = state[s];
	i = *p++;
	if (i == 0) return;
	printf("%4d", *p++);
	for (j = 1; j < i; j++) {
		printf(", %4d", *p++);
		if (j % 30 == 0)putchar('\n');
	}
	putchar('\n');
	return;
}
# endif
int member(int d,char* t){
	 int c;
	 char* s;
	c = d;
	s = t;
	c = cindex[c];
	while (*s)
		if (*s++ == c) return(1);
	return(0);
}
# ifdef DEBUG
stprt(i)
int i; {
	 int p, t;
	printf("State %d:", i);
	/* print actions, if any */
	t = atable[i];
	if (t != -1)printf(" final");
	putchar('\n');
	if (cpackflg[i] == TRUE)printf("backup char in use\n");
	if (sfall[i] != -1)printf("fall back state %d\n", sfall[i]);
	p = gotof[i];
	if (p == -1) return;
	printf("(%d transitions)\n", nexts[p]);
	while (nchar[p]) {
		charc = 0;
		if (nexts[p + 1] >= 0)
			printf("%d\t", nexts[p + 1]);
		else printf("err\t");
		allprint(nchar[p++]);
		while (nexts[p] == nexts[p + 1] && nchar[p]) {
			if (charc > LINESIZE) {
				charc = 0;
				printf("\n\t");
			}
			allprint(nchar[p++]);
		}
		putchar('\n');
	}
	putchar('\n');
	return;
}
# endif

void acompute(int s){	/* compute action list = set of poss. actions */
	 int* p, i, j;
	int cnt, m;
	int temp[300], k, neg[300], n;
	k = 0;
	n = 0;
	p = state[s];
	cnt = *p++;
	if (cnt > 300)
		error("Too many positions for one state - acompute");
	for (i = 0; i < cnt; i++) {
		if (name[*p] == FINAL)temp[k++] = gint(left[*p]);
		else if (name[*p] == S1FINAL) {
			temp[k++] = gint(left[*p]);
			if (gint(left[*p]) > NACTIONS) error("Too many right contexts");
			extra[gint(left[*p])] = 1;
		}
		else if (name[*p] == S2FINAL)neg[n++] = gint(left[*p]);
		p++;
	}
	atable[s] = -1;
	if (k < 1 && n < 1) return;
# ifdef DEBUG
	if (debug) printf("final %d actions:", s);
# endif
	/* sort action list */
	for (i = 0; i < k; i++)
		for (j = i + 1; j < k; j++)
			if (temp[j] < temp[i]) {
				m = temp[j];
				temp[j] = temp[i];
				temp[i] = m;
			}
	/* remove dups */
	for (i = 0; i < k - 1; i++)
		if (temp[i] == temp[i + 1]) temp[i] = 0;
	/* copy to permanent quarters */
	atable[s] = aptr;
# ifdef DEBUG
	if (!ratfor)fprintf(fout, "/* actions for state %d */", s);
# endif
	putc('\n', fout);
	for (i = 0; i < k; i++)
		if (temp[i] != 0) {
			 fprintf(fout, "%d,\n", temp[i]);
# ifdef DEBUG
			if (debug)
				printf("%d ", temp[i]);
# endif
			aptr++;
		}
	for (i = 0; i < n; i++) {		/* copy fall back actions - all neg */
		 fprintf(fout, "%d,\n", neg[i]);
		aptr++;
# ifdef DEBUG
		if (debug)printf("%d ", neg[i]);
# endif
	}
# ifdef DEBUG
	if (debug)putchar('\n');
# endif
	fprintf(fout, "0,\n");
	aptr++;
}
# ifdef DEBUG
pccl() {
	/* print character class sets */
	 int i, j;
	printf("char class intersection\n");
	for (i = 0; i < ccount; i++) {
		charc = 0;
		printf("class %d:\n\t", i);
		for (j = 1; j < NCH; j++)
			if (cindex[j] == i) {
				allprint(j);
				if (charc > LINESIZE) {
					printf("\n\t");
					charc = 0;
				}
			}
		putchar('\n');
	}
	charc = 0;
	printf("match:\n");
	for (i = 0; i < NCH; i++) {
		allprint(match[i]);
		if (charc > LINESIZE) {
			putchar('\n');
			charc = 0;
		}
	}
	putchar('\n');
	return;
}
# endif
void mkmatch() {
	 int i;
	 std::array<char, NCH> tab;
	for (i = 0; i < ccount; i++)
		tab[i] = 0;
	for (i = 1; i < NCH; i++)
		if (tab[cindex[i]] == 0)
			tab[cindex[i]] = i;
	/* tab[i] = principal char for new ccl i */
	for (i = 1; i < NCH; i++)
		match[i] = tab[cindex[i]];
}
void layout() {
	/* format and output final program's tables */
	 int i, j, k;
	int  top, bot, startup, omin;
	startup = 0;
	for (i = 0; i < outsize; i++)
		verify[i] = advance[i] = 0;
	omin = 0;
	yytop = 0;
	for (i = 0; i <= stnum; i++) {	/* for each state */
		j = gotof[i];
		if (j == -1) {
			stoff[i] = 0;
			continue;
		}
		bot = j;
		while (nchar[j])j++;
		top = j - 1;
# if DEBUG
		if (debug)
		{
			printf("State %d: (layout)\n", i);
			for (j = bot; j <= top; j++)
			{
				printf("  %o", nchar[j]);
				if (j % 10 == 0) putchar('\n');
			}
			putchar('\n');
		}
# endif
		while (verify[omin + ZCH]) omin++;
		startup = omin;
# if DEBUG
		if (debug) printf("bot,top %d, %d startup begins %d\n", bot, top, startup);
# endif
		if (chset) {
			do {
				startup += 1;
				if (startup > outsize - ZCH)
					error("output table overflow");
				for (j = bot; j <= top; j++) {
					k = startup + ctable[nchar[j]];
					if (verify[k])break;
				}
			} while (j <= top);
# if DEBUG
			if (debug) printf(" startup will be %d\n", startup);
# endif
			/* have found place */
			for (j = bot; j <= top; j++) {
				k = startup + ctable[nchar[j]];
				if (ctable[nchar[j]] <= 0)
					printf("j %d nchar %d ctable.nch %d\n", j, nchar[j], ctable[nchar[k]]);
				verify[k] = i + 1;			/* state number + 1*/
				advance[k] = nexts[j + 1] + 1;		/* state number + 1*/
				if (yytop < k) yytop = k;
			}
		}
		else {
			do {
				startup += 1;
				if (startup > outsize - ZCH)
					error("output table overflow");
				for (j = bot; j <= top; j++) {
					k = startup + nchar[j];
					if (verify[k])break;
				}
			} while (j <= top);
			/* have found place */
# if DEBUG
			if (debug) printf(" startup going to be %d\n", startup);
# endif
			for (j = bot; j <= top; j++) {
				k = startup + nchar[j];
				verify[k] = i + 1;			/* state number + 1*/
				advance[k] = nexts[j + 1] + 1;		/* state number + 1*/
				if (yytop < k) yytop = k;
			}
		}
		stoff[i] = startup;
	}

	/* stoff[i] = offset into verify, advance for trans for state i */
	/* put out yywork */

	fprintf(fout, "# define YYTYPE %s\n", stnum + 1 > NCH ? "int" : "char");
	fprintf(fout, "struct yywork { YYTYPE verify, advance; } yycrank[] = {\n");
	for (i = 0; i <= yytop; i += 4) {
		for (j = 0; j < 4; j++) {
			k = i + j;
			if (verify[k])
				fprintf(fout, "%d,%d,\t", verify[k], advance[k]);
			else
				fprintf(fout, "0,0,\t");
		}
		putc('\n', fout);
	}
	fprintf(fout, "0,0};\n");

	/* put out yysvec */

	fprintf(fout, "struct yysvf yysvec[] = {\n");
	fprintf(fout, "0,\t0,\t0,\n");
	for (i = 0; i <= stnum; i++) {	/* for each state */
		if (cpackflg[i])stoff[i] = -stoff[i];
		fprintf(fout, "yycrank+%d,\t", stoff[i]);
		if (sfall[i] != -1)
			fprintf(fout, "yysvec+%d,\t", sfall[i] + 1);	/* state + 1 */
		else fprintf(fout, "0,\t\t");
		if (atable[i] != -1)
			fprintf(fout, "yyvstop+%d,", atable[i]);
		else fprintf(fout, "0,\t");
# ifdef DEBUG
		fprintf(fout, "\t\t/* state %d */", i);
# endif
		putc('\n', fout);
	}
	fprintf(fout, "0,\t0,\t0};\n");

	/* put out yymatch */

	fprintf(fout, "struct yywork *yytop = yycrank+%d;\n", yytop);
	fprintf(fout, "struct yysvf *yybgin = yysvec+1;\n");
	if (optim) {
		fprintf(fout, "char yymatch[] = {\n");
		if (chset == 0) /* no chset, put out in normal order */
		{
			for (i = 0; i < NCH; i += 8) {
				for (j = 0; j < 8; j++) {
					int fbch;
					fbch = match[i + j];
					if (printable(fbch) && fbch != '\'' && fbch != '\\')
						fprintf(fout, "'%c' ,", fbch);
					else fprintf(fout, "0%-3o,", fbch);
				}
				putc('\n', fout);
			}
		}
		else
		{
			int* fbarr;
			fbarr = (int*)calloc(2 * NCH, sizeof(*fbarr));
			if (fbarr != NULL) {
				for (i = 0; i < ZCH; i++)
					fbarr[i] = 0;
				for (i = 0; i < NCH; i++)
					fbarr[ctable[i]] = ctable[match[i]];
				for (i = 0; i < ZCH; i += 8)
				{
					for (j = 0; j < 8; j++)
						fprintf(fout, "0%-3o,", fbarr[i + j]);
					putc('\n', fout);
				}
				cfree((char*)fbarr, 2 * NCH, 1);
			} else
				error("No space for char table reverse", 0);

		}
		fprintf(fout, "0};\n");
	}
	/* put out yyextra */
	fprintf(fout, "char yyextra[] = {\n");
	for (i = 0; i < casecount; i += 8) {
		for (j = 0; j < 8; j++)
			fprintf(fout, "%d,", i + j < NACTIONS ?
				extra[i + j] : 0);
		putc('\n', fout);
	}
	fprintf(fout, "0};\n");
	return;
}
void rprint(int*a,char* s,int n){
	 int i;
	fprintf(fout, "block data\n");
	fprintf(fout, "common /L%s/ %s\n", s, s);
	fprintf(fout, "define S%s %d\n", s, n);
	fprintf(fout, "integer %s (S%s)\n", s, s);
	for (i = 1; i <= n; i++)
	{
		if (i % 8 == 1) fprintf(fout, "data ");
		fprintf(fout, "%s (%d)/%d/", s, i, a[i]);
		fprintf(fout, (i % 8 && i < n) ? ", " : "\n");
	}
	fprintf(fout, "end\n");
}
void shiftr(int* a,int  n) {
	int i;
	for (i = n; i >= 0; i--)
		a[i + 1] = a[i];
}
void upone(int* a,int n){
	int i;
	for (i = 0; i <= n; i++)
		a[i]++;
}

void bprint(char* a,char* s,int n){
	 int i, j, k;
	fprintf(fout, "block data\n");
	fprintf(fout, "common /L%s/ %s\n", s, s);
	fprintf(fout, "define S%s %d\n", s, n);
	fprintf(fout, "integer %s (S%s)\n", s, s);
	for (i = 1; i < n; i += 8) {
		fprintf(fout, "data %s (%d)/%d/", s, i, a[i]);
		for (j = 1; j < 8; j++) {
			k = i + j;
			if (k < n)fprintf(fout, ", %s (%d)/%d/", s, k, a[k]);
		}
		putc('\n', fout);
	}
	fprintf(fout, "end\n");
}
# ifdef PP
padd(array, n)
int** array;
int n; {
	 int i, * j, k;
	array[n] = nxtpos;
	if (count == 0) {
		*nxtpos++ = 0;
		return;
	}
	for (i = tptr - 1; i >= 0; i--) {
		j = array[i];
		if (j && *j++ == count) {
			for (k = 0; k < count; k++)
				if (!tmpstat[*j++])break;
			if (k >= count) {
				array[n] = array[i];
				return;
			}
		}
	}
	add(array, n);
	return;
}
# endif

// parser.y, hope this works


int yylex() {
	 char* p;
	 int c, i;
	char* t, * xp;
	int n, j, k, x;
	static int sectbegin;
	static char token[TOKENSIZE];
	static int iter;

# ifdef DEBUG
	yylval.i = 0;
# endif

	if (sect == DEFSECTION) {		/* definitions section */
		while (!eof) {
			if (prev == '\n') {		/* next char is at beginning of line */
				getl(p = buf);
				switch (*p) {
				case '%':
					switch (c = *(p + 1)) {
					case '%':
						lgate();
						fprintf(fout, "# ");
						fprintf(fout, "define YYNEWLINE %d\n", ctable['\n']);
						fprintf(fout, "yylex(){\nint nstr; extern int yyprevious;\n");
						sectbegin = TRUE;
						i = treesize * (sizeof(*name) + sizeof(*left) +
							sizeof(*right) + sizeof(*nullstr) + sizeof(*parent)) + ALITTLEEXTRA;
						c = (int)myalloc(i, 1);
						if (c == 0)
							error("Too little core for parse tree");
						p = (char*)c;
						cfree((char*)p, i, 1);
						name = (int*)myalloc(treesize, sizeof(*name));
						left = (left_t*)myalloc(treesize, sizeof(left_t));
						right = (int*)myalloc(treesize, sizeof(*right));
						nullstr = myalloc(treesize, sizeof(*nullstr));
						parent = (int*)myalloc(treesize, sizeof(*parent));
						if (name == 0 || left == 0 || right == 0 || parent == 0 || nullstr == 0)
							error("Too little core for parse tree");
						return(freturn(DELIM));
					case 'p': case 'P':	/* has overridden number of positions */
						while (*p && !digit(*p))p++;
						maxpos = siconv(p);
# ifdef DEBUG
						if (debug) printf("positions (%%p) now %d\n", maxpos);
# endif
						if (report == 2)report = 1;
						continue;
					case 'n': case 'N':	/* has overridden number of states */
						while (*p && !digit(*p))p++;
						nstates = siconv(p);
# ifdef DEBUG
						if (debug)printf(" no. states (%%n) now %d\n", nstates);
# endif
						if (report == 2)report = 1;
						continue;
					case 'e': case 'E':		/* has overridden number of tree nodes */
						while (*p && !digit(*p))p++;
						treesize = siconv(p);
# ifdef DEBUG
						if (debug) printf("treesize (%%e) now %d\n", treesize);
# endif
						if (report == 2)report = 1;
						continue;
					case 'o': case 'O':
						while (*p && !digit(*p))p++;
						outsize = siconv(p);
						if (report == 2) report = 1;
						continue;
					case 'a': case 'A':		/* has overridden number of transitions */
						while (*p && !digit(*p))p++;
						if (report == 2)report = 1;
						ntrans = siconv(p);
# ifdef DEBUG
						if (debug)printf("N. trans (%%a) now %d\n", ntrans);
# endif
						continue;
					case 'k': case 'K': /* overriden packed char classes */
						while (*p && !digit(*p))p++;
						if (report == 2) report = 1;
						cfree((char*)pchar, pchlen, sizeof(*pchar));
						pchlen = siconv(p);
# ifdef DEBUG
						if (debug) printf("Size classes (%%k) now %d\n", pchlen);
# endif
						pchar = pcptr = myalloc(pchlen, sizeof(*pchar));
						continue;
					case 't': case 'T': 	/* character set specifier */
						ZCH = atoi(p + 2);
						if (ZCH < NCH) ZCH = NCH;
						if (ZCH > 2 * NCH) error("ch table needs redeclaration");
						chset = TRUE;
						for (i = 0; i < ZCH; i++)
							ctable[i] = 0;
						while (getl(p) && strcmp(p, "%T") != 0 && strcmp(p, "%t") != 0) {
							if ((n = siconv(p)) <= 0 || n > ZCH) {
								warning("Character value %d out of range", n);
								continue;
							}
							while (!space(*p) && *p) p++;
							while (space(*p)) p++;
							t = p;
							while (*t) {
								c = ctrans(&t);
								if (ctable[c]) {
									if (printable(c))
										warning("Character '%c' used twice", c);
									else
										warning("Character %o used twice", c);
								}
								else ctable[c] = n;
								t++;
							}
							p = buf;
						}
						{
							char chused[2 * NCH]; int kr;
							for (i = 0; i < ZCH; i++)
								chused[i] = 0;
							for (i = 0; i < NCH; i++)
								chused[ctable[i]] = 1;
							for (kr = i = 1; i < NCH; i++)
								if (ctable[i] == 0)
								{
									while (chused[kr] == 0)
										kr++;
									ctable[i] = kr;
									chused[kr] = 1;
								}
						}
						lgate();
						continue;
					case '{':
						lgate();
						while (getl(p) && strcmp(p, "%}") != 0)
							fprintf(fout, "%s\n", p);
						if (p[0] == '%') continue;
						error("Premature eof");
					case 's': case 'S':		/* start conditions */
						lgate();
						while (*p && (*p == ' ' || *p == '\t')) p++;
						n = TRUE;
						while (n) {
							while (*p && (*p == ' ' || *p == '\t')) p++;
							t = p;
							while (*p && (*p == ' ' || *p == '\t'))p++;
							if (!*p) n = FALSE;
							*p++ = 0;
							if (*t == 0) continue;
							i = sptr * 2;
							fprintf(fout, "define %s %d\n", t, i);
							strcpy(sp,t);
							sname[sptr++] = sp;
							sname[sptr] = 0;	/* required by lookup */
							if (sptr >= STARTSIZE)
								error("Too many start conditions");
							sp += slength(sp) + 1;
							if (sp >= schar + STARTCHAR)
								error("Start conditions too long");
						}
						continue;
					default:
						warning("Invalid request %s", p);
						continue;
					}	/* end of switch after seeing '%' */
				case ' ': case '\t':		/* must be code */
					lgate();
					fprintf(fout, "%s\n", p);
					continue;
				default:		/* definition */
					while (*p && !space(*p)) p++;
					if (*p == 0)
						continue;
					prev = *p;
					*p = 0;
					bptr = p + 1;
					yylval.cp = buf;
					if (digit(buf[0]))
						warning("Substitution strings may not begin with digits");
					return(freturn(STR));
				}
			}
			/* still sect 1, but prev != '\n' */
			else {
				p = bptr;
				while (*p && space(*p)) p++;
				if (*p == 0)
					warning("No translation given - null string assumed");
				strcpy(token, p);
				yylval.cp = token;
				prev = '\n';
				return(freturn(STR));
			}
		}
		/* end of section one processing */
	}
	else if (sect == RULESECTION) {		/* rules and actions */
		while (!eof) {
			switch (c = gch()) {
			case '\0':
				return(freturn(0));
			case '\n':
				if (prev == '\n') continue;
				x = NEWE;
				break;
			case ' ':
			case '\t':
				if (sectbegin == TRUE) {
					cpyact();
					while ((c = gch()) && c != '\n');
					continue;
				}
				if (!funcflag)phead2();
				funcflag = TRUE;
				fprintf(fout, "case %d:\n", casecount);
				if (cpyact()) {
					fprintf(fout, "break;\n");
				}
				while ((c = gch()) && c != '\n');
				if (peek == ' ' || peek == '\t' || sectbegin == TRUE) {
					warning("Executable statements should occur right after %%");
					continue;
				}
				x = NEWE;
				break;
			case '%':
				if (prev != '\n') goto character;
				if (peek == '{') {	/* included code */
					getl(buf);
					while (!eof && getl(buf) && strcmp("%}", buf) != 0)
						fprintf(fout, "%s\n", buf);
					continue;
				}
				if (peek == '%') {
					c = gch();
					c = gch();
					x = DELIM;
					break;
				}
				goto character;
			case '|':
				if (peek == ' ' || peek == '\t' || peek == '\n') {
					fprintf(fout, "case %d:\n", casecount++);
					continue;
				}
				x = '|';
				break;
			case '$':
				if (peek == '\n' || peek == ' ' || peek == '\t' || peek == '|' || peek == '/') {
					x = c;
					break;
				}
				goto character;
			case '^':
				if (prev != '\n' && scon != TRUE) goto character;	/* valid only at line begin */
				x = c;
				break;
			case '?':
			case '+':
			case '.':
			case '*':
			case '(':
			case ')':
			case ',':
			case '/':
				x = c;
				break;
			case '}':
				iter = FALSE;
				x = c;
				break;
			case '{':	/* either iteration or definition */
				if (digit(c = gch())) {	/* iteration */
					iter = TRUE;
				ieval:
					i = 0;
					while (digit(c)) {
						token[i++] = c;
						c = gch();
					}
					token[i] = 0;
					yylval.i = siconv(token);
					munput('c', c);
					x = ITER;
					break;
				}
				else {		/* definition */
					i = 0;
					while (c && c != '}') {
						token[i++] = c;
						c = gch();
					}
					token[i] = 0;
					i = lookup(token, def);
					if (i < 0)
						warning("Definition %s not found", token);
					else
						munput('s', subs[i]);
					continue;
				}
			case '<':		/* start condition ? */
				if (prev != '\n')		/* not at line begin, not start */
					goto character;
				t = slptr;
				do {
					i = 0;
					c = gch();
					while (c != ',' && c && c != '>') {
						token[i++] = c;
						c = gch();
					}
					token[i] = 0;
					if (i == 0)
						goto character;
					i = lookup(token, sname);
					if (i < 0) {
						warning("Undefined start condition %s", token);
						continue;
					}
					*slptr++ = i + 1;
				} while (c && c != '>');
				*slptr++ = 0;
				/* check if previous value re-usable */
				for (xp = slist; xp < t; )
				{
					if (strcmp(xp, t) == 0)
						break;
					while (*xp++);
				}
				if (xp < t)
				{
					/* re-use previous pointer to string */
					slptr = t;
					t = xp;
				}
				if (slptr > slist + STARTSIZE)		/* note not packed ! */
					error("Too many start conditions used");
				yylval.cp = t;
				x = SCON;
				break;
			case '"':
				i = 0;
				while ((c = gch()) && c != '"' && c != '\n') {
					if (c == '\\') c = usescape(c = gch());
					token[i++] = c;
					if (i > TOKENSIZE) {
						warning("String too long");
						i = TOKENSIZE - 1;
						break;
					}
				}
				if (c == '\n') {
					yyline--;
					warning("Non-terminated string");
					yyline++;
				}
				token[i] = 0;
				if (i == 0)x = NULLS;
				else if (i == 1) {
					yylval.i = token[0];
					x = CHAR;
				}
				else {
					yylval.cp = token;
					x = STR;
				}
				break;
			case '[':
				for (i = 1; i < NCH; i++) symbol[i] = 0;
				x = CCL;
				if ((c = gch()) == '^') {
					x = NCCL;
					c = gch();
				}
				while (c != ']' && c) {
					if (c == '\\') c = usescape(c = gch());
					symbol[c] = 1;
					j = c;
					if ((c = gch()) == '-' && peek != ']') {		/* range specified */
						c = gch();
						if (c == '\\') c = usescape(c = gch());
						k = c;
						if (j > k) {
							n = j;
							j = k;
							k = n;
						}
						if (!(('A' <= j && k <= 'Z') ||
							('a' <= j && k <= 'z') ||
							('0' <= j && k <= '9')))
							warning("Non-portable Character Class");
						for (n = j + 1; n <= k; n++)
							symbol[n] = 1;		/* implementation dependent */
						c = gch();
					}
				}
				/* try to pack ccl's */
				i = 0;
				for (j = 0; j < NCH; j++)
					if (symbol[j])token[i++] = j;
				token[i] = 0;
				p = ccptr;
				if (optim) {
					p = ccl;
					while (p < ccptr && strcmp(token, p) != 0)p++;
				}
				if (p < ccptr)	/* found it */
					yylval.cp = p;
				else {
					yylval.cp = ccptr;
					strcpy(ccptr,token);
					ccptr += slength(token) + 1;
					if (ccptr >= ccl + CCLSIZE)
						error("Too many large character classes");
				}
				cclinter(x == CCL);
				break;
			case '\\':
				c = usescape(c = gch());
			default:
			character:
				if (iter) {	/* second part of an iteration */
					iter = FALSE;
					if ('0' <= c && c <= '9')
						goto ieval;
				}
				if (alpha(peek)) {
					i = 0;
					yylval.cp = token;
					token[i++] = c;
					while (alpha(peek))
						token[i++] = gch();
					if (peek == '?' || peek == '*' || peek == '+')
						munput(token[--i]);
					token[i] = 0;
					if (i == 1) {
						yylval.i = token[0];
						x = CHAR;
					}
					else x = STR;
				}
				else {
					yylval.i = c;
					x = CHAR;
				}
			}
			scon = FALSE;
			if (x == SCON)scon = TRUE;
			sectbegin = FALSE;
			return(freturn(x));
		}
	}
	/* section three */
	ptail();
#ifdef DEBUG
	if (debug)
		fprintf(fout, "\n/*this comes from section three - debug */\n");
#endif
	while (getl(buf) && !eof)
		fprintf(fout, "%s\n", buf);
	return (freturn(0));
}
/* end of yylex */
# ifdef DEBUG
void freturn(int i){
	if (yydebug) {
		printf("now return ");
		if (i < NCH) allprint(i);
		else printf("%d", i);
		printf("   yylval = ");
		switch (i) {
		case STR: case CCL: case NCCL:
			strpt(yylval.cp);
			break;
		case CHAR:
			allprint(yylval.i);
			break;
		default:
			printf("%d", yylval.i);
			break;
		}
		putchar('\n');
	}
	return(i);
}
# endif

int main(int argc,const char** argv){
	 int i;
# ifdef DEBUG
	signal(10, buserr);
	signal(11, segviol);
# endif
	while (argc > 1 && argv[1][0] == '-') {
		i = 0;
		while (argv[1][++i]) {
			switch (argv[1][i]) {
# ifdef DEBUG
			case 'd': debug++; break;
			case 'y': yydebug = TRUE; break;
# endif

			case 't': case 'T':
				fout = stdout;
				errorf = stderr;
				break;
			case 'v': case 'V':
				report = 1;
				break;
			case 'n': case 'N':
				report = 0;
				break;
			default:
				warning("Unknown option %c", argv[1][i]);
			}
		}
		argc--;
		argv++;
	}
	sargc = argc;
	sargv = argv;

	gch();
	/* may be gotten: def, subs, sname, schar, ccl, dchar */
	get1core();
	/* may be gotten: name, left, right, nullstr, parent */
	strcpy(sp,"INITIAL");
	sname[0] = sp;
	sp += strlen("INITIAL") + 1;
	sname[1] = 0;
	if (yyparse(0)) exit(1);	/* error return code */
		/* may be disposed of: def, subs, dchar */
	free1core();
	/* may be gotten: tmpstat, foll, positions, gotof, nexts, nchar, state, atable, sfall, cpackflg */
	get2core();
	ptail();
	mkmatch();
# ifdef DEBUG
	if (debug) pccl();
# endif
	sect = ENDSECTION;
	if (tptr > 0)cfoll(tptr - 1);
# ifdef DEBUG
	if (debug)pfoll();
# endif
	cgoto();
# ifdef DEBUG
	if (debug) {
		printf("Print %d states:\n", stnum + 1);
		for (i = 0; i <= stnum; i++)stprt(i);
	}
# endif
	/* may be disposed of: positions, tmpstat, foll, state, name, left, right, parent, ccl, schar, sname */
	/* may be gotten: verify, advance, stoff */
	free2core();
	get3core();
	layout();
	/* may be disposed of: verify, advance, stoff, nexts, nchar,
		gotof, atable, ccpackflg, sfall */
# ifdef DEBUG
	free3core();
# endif
	const char* cname = "ansiform";
	fother = fopen(cname, "r");
	if (fother != NULL) {
		while ((i = fgetc(fother)) != EOF)
			putc(i, fout);

		fclose(fother);
	} else
		error("Lex driver missing, file %s",  cname);

	fclose(fout);
	if (
# ifdef DEBUG
		debug ||
# endif
		report == 1)statistics();
	fclose(stdout);
	fclose(stderr);
	exit(0);	/* success return code */
}


void get1core() {
	ccptr = ccl = myalloc(CCLSIZE, sizeof(*ccl));
	pcptr = pchar = myalloc(pchlen, sizeof(*pchar));
	def = (char**)myalloc(DEFSIZE, sizeof(*def));
	subs = (char**)myalloc(DEFSIZE, sizeof(*subs));
	dp = dchar = myalloc(DEFCHAR, sizeof(*dchar));
	sname = (char**)myalloc(STARTSIZE, sizeof(*sname));
	sp = schar = myalloc(STARTCHAR, sizeof(*schar));
	if (ccl == 0 || def == 0 || subs == 0 || dchar == 0 || sname == 0 || schar == 0)
		error("Too little core to begin");
}
void free1core() {
	cfree((char*)def, DEFSIZE, sizeof(*def));
	cfree((char*)subs, DEFSIZE, sizeof(*subs));
	cfree((char*)dchar, DEFCHAR, sizeof(*dchar));
}
void get2core() {
	gotof = (int*)myalloc(nstates, sizeof(*gotof));
	nexts = (int*)myalloc(ntrans, sizeof(*nexts));
	nchar = myalloc(ntrans, sizeof(*nchar));
	state = (int**)myalloc(nstates, sizeof(*state));
	atable = (int*)myalloc(nstates, sizeof(*atable));
	sfall = (int*)myalloc(nstates, sizeof(*sfall));
	cpackflg = myalloc(nstates, sizeof(*cpackflg));
	tmpstat = myalloc(tptr + 1, sizeof(*tmpstat));
	foll = (int**)myalloc(tptr + 1, sizeof(*foll));
	nxtpos = positions = (int*)myalloc(maxpos, sizeof(*positions));
	if (tmpstat == 0 || foll == 0 || positions == 0 ||
		gotof == 0 || nexts == 0 || nchar == 0 || state == 0 || atable == 0 || sfall == 0 || cpackflg == 0)
		error("Too little core for state generation");
	for (int i = 0; i <= tptr; i++)foll[i] = 0;
}
void free2core() {
	cfree((char*)positions, maxpos, sizeof(*positions));
	cfree((char*)tmpstat, tptr + 1, sizeof(*tmpstat));
	cfree((char*)foll, tptr + 1, sizeof(*foll));
	cfree((char*)name, treesize, sizeof(*name));
	cfree((char*)left, treesize, sizeof(*left));
	cfree((char*)right, treesize, sizeof(*right));
	cfree((char*)parent, treesize, sizeof(*parent));
	cfree((char*)nullstr, treesize, sizeof(*nullstr));
	cfree((char*)state, nstates, sizeof(*state));
	cfree((char*)sname, STARTSIZE, sizeof(*sname));
	cfree((char*)schar, STARTCHAR, sizeof(*schar));
	cfree((char*)ccl, CCLSIZE, sizeof(*ccl));
}

void get3core() {
	verify = (int*)myalloc(outsize, sizeof(*verify));
	advance = (int*)myalloc(outsize, sizeof(*advance));
	stoff = (int*)myalloc(stnum + 2, sizeof(*stoff));
	if (verify == 0 || advance == 0 || stoff == 0)
		error("Too little core for final packing");
}
# ifdef DEBUG
free3core() {
	cfree((char*)advance, outsize, sizeof(*advance));
	cfree((char*)verify, outsize, sizeof(*verify));
	cfree((char*)stoff, stnum + 1, sizeof(*stoff));
	cfree((char*)gotof, nstates, sizeof(*gotof));
	cfree((char*)nexts, ntrans, sizeof(*nexts));
	cfree((char*)nchar, ntrans, sizeof(*nchar));
	cfree((char*)atable, nstates, sizeof(*atable));
	cfree((char*)sfall, nstates, sizeof(*sfall));
	cfree((char*)cpackflg, nstates, sizeof(*cpackflg));
}
# endif

# ifdef DEBUG
buserr() {
	fflush(errorf);
	fflush(fout);
	fflush(stdout);
	fprintf(errorf, "Bus error\n");
	if (report == 1)statistics();
	fflush(errorf);
}
segviol() {
	fflush(errorf);
	fflush(fout);
	fflush(stdout);
	fprintf(errorf, "Segmentation violation\n");
	if (report == 1)statistics();
	fflush(errorf);
}
# endif
