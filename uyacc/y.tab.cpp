// include def.c
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>


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


extern int sargc;
extern char** sargv;
extern char buf[520];
extern int ratfor;		/* 1 = ratfor, 0 = C */
extern int yyline;		/* line number of file */
extern int sect;
extern int eof;
extern int lgatflg;
extern int divflg;
extern int funcflag;
extern int pflag;
extern int casecount;
extern int chset;	/* 1 = char set modified */
extern FILE* fin, * fout, * fother, * errorf;
extern int fptr;
extern char* ratname, * cname;
extern int prev;	/* previous input character */
extern int pres;	/* present input character */
extern int peek;	/* next input character */
extern int* name;
extern int* left;
extern int* right;
extern int* parent;
extern char* nullstr;
extern int tptr;
extern char pushc[TOKENSIZE];
extern char* pushptr;
extern char slist[STARTSIZE];
extern char* slptr;
extern char** def, ** subs, * dchar;
extern char** sname, * schar;
extern char* ccl;
extern char* ccptr;
extern char* dp, * sp;
extern int dptr, sptr;
extern char* bptr;		/* store input position */
extern char* tmpstat;
extern int count;
extern int** foll;
extern int* nxtpos;
extern int* positions;
extern int* gotof;
extern int* nexts;
extern char* nchar;
extern int** state;
extern int* sfall;		/* fallback state num */
extern char* cpackflg;		/* true if state has been character packed */
extern int* atable, aptr;
extern int nptr;
extern char symbol[NCH];
extern char cindex[NCH];
extern int xstate;
extern int stnum;
extern int ctable[];
extern int ZCH;
extern int ccount;
extern char match[NCH];
extern char extra[NACTIONS];
extern char* pcptr, * pchar;
extern int pchlen;
extern int nstates, maxpos;
extern int yytop;
extern int report;
extern int ntrans, treesize, outsize;
extern long rcount;
extern int optim;
extern int* verify, * advance, * stoff;
extern int scon;
extern char* psave;
char* getl(char* p);
char* myalloc(int a, int b);

extern int buserr(), segviol();

using left_t = std::variant<int, char*>;
#define gint(V) std::get<int>(V)
#define gstr(V) std::get<char*>(V)
void error("Too many definitions",...);
int mn1(int a, left_t d);
int mn2(int a, int d, int c);
int mn1(int a, left_t d);
int mn0(int a);

int yylex();
void yyerror(const char* s)
{
	fprintf(stderr, "line %d: %s\n", yyline, s);
}



# define CHAR 257
# define CCL 258
# define NCCL 259
# define STR 260
# define DELIM 261
# define SCON 262
# define ITER 263
# define NEWE 264
# define NULLS 265
# define CAT 266

# line 24 "parser.y"
#define YYSTYPE union _yystype_
union _yystype_
{
	int	i;
	char	*cp;
};

#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;

# line 34 "parser.y"
int i;
int j,k;
int g;
char *p;
# define YYERRCODE 256

# line 230 "parser.y"

yytabelem yyexca[] ={
-1, 0,
	260, 8,
	261, 8,
	-2, 0,
-1, 1,
	0, -1,
	-2, 0,
-1, 5,
	0, 6,
	-2, 0,
-1, 8,
	0, 6,
	-2, 0,
	};
# define YYNPROD 33
# define YYLAST 291
yytabelem yyact[]={

    34,    45,    35,    23,    21,    43,    27,    28,     4,    34,
    16,    32,    41,    21,    46,    27,    28,    10,    34,    16,
    32,     8,    21,     9,    27,    28,    25,    29,    16,     6,
     7,     3,    24,    34,    27,    28,    29,    21,    11,    27,
    28,     2,     5,    16,     1,    29,    21,     0,    27,    28,
     0,     0,    16,     0,     0,    29,    21,     0,    20,     0,
    29,     0,    16,     0,     0,    21,     0,    20,     0,    29,
     0,    16,     0,     0,    31,     0,    20,     0,     0,     0,
    12,     0,     0,    12,     0,     0,     0,     0,    30,     0,
     0,    20,     0,    42,    36,    37,    38,    30,     0,     0,
     0,     0,     0,     0,     0,    39,    30,    40,     0,     0,
    20,     0,     0,     0,     0,     0,     0,     0,     0,    20,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    44,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    14,    17,    18,    15,     0,    19,    33,     0,    22,
    14,    17,    18,    15,     0,    19,    33,    26,    22,    14,
    17,    18,    15,     0,     0,    33,     0,    22,     0,     0,
     0,     0,     0,     0,    14,    17,    18,    15,     0,     0,
    33,     0,    22,    14,    17,    18,    15,     0,     0,    33,
     0,    22,    13,    14,    17,    18,    15,     7,    19,     0,
     0,    22,    14,    17,    18,    15,     0,    19,     0,     0,
    22 };
yytabelem yypact[]={

  -248, -1000, -1000,  -231, -1000,    16,  -257, -1000,    16, -1000,
 -1000, -1000,   -27,  -262, -1000, -1000, -1000, -1000, -1000,    25,
    25,    25, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
    25,    -8,    25,   -32, -1000, -1000,   -18,     6,   -36,    -3,
   -18,  -124, -1000, -1000,  -111, -1000, -1000 };
yytabelem yypgo[]={

     0,    44,    41,    31,    38,    21,    23,    17,    74 };
yytabelem yyr1[]={

     0,     1,     2,     2,     2,     6,     6,     3,     3,     4,
     5,     5,     7,     7,     8,     8,     8,     8,     8,     8,
     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
     8,     8,     8 };
yytabelem yyr2[]={

     0,     3,     8,     7,     3,     2,     0,     7,     0,     3,
     5,     3,     5,     5,     3,     3,     3,     3,     3,     5,
     5,     5,     7,     5,     7,    11,     7,     9,     5,     5,
     5,     7,     3 };
yytabelem yychk[]={

 -1000,    -1,    -2,    -3,   256,    -4,   260,   261,    -5,    -6,
    -7,    -4,    -8,   256,   257,   260,    46,   258,   259,   262,
    94,    40,   265,   260,    -6,    -7,   264,    42,    43,    63,
   124,    -8,    47,   263,    36,   264,    -8,    -8,    -8,    -8,
    -8,    44,   125,    41,   263,   125,   125 };
yytabelem yydef[]={

    -2,    -2,     1,     0,     4,    -2,     0,     9,    -2,     3,
    11,     5,     0,     0,    14,    15,    16,    17,    18,     0,
     0,     0,    32,     7,     2,    10,    12,    19,    20,    21,
     0,    23,     0,     0,    30,    13,    28,    29,     0,    22,
    24,     0,    26,    31,     0,    27,    25 };
typedef struct { const char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	1	/* allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"CHAR",	257,
	"CCL",	258,
	"NCCL",	259,
	"STR",	260,
	"DELIM",	261,
	"SCON",	262,
	"ITER",	263,
	"NEWE",	264,
	"NULLS",	265,
	"/",	47,
	"|",	124,
	"$",	36,
	"^",	94,
	"(",	40,
	".",	46,
	"CAT",	266,
	"*",	42,
	"+",	43,
	"?",	63,
	"-unknown-",	-1	/* ends search */
};

const char * yyreds[] =
{
	"-no such reduction-",
	"acc : lexinput",
	"lexinput : defns delim prods end",
	"lexinput : defns delim end",
	"lexinput : error",
	"end : delim",
	"end : /* empty */",
	"defns : defns STR STR",
	"defns : /* empty */",
	"delim : DELIM",
	"prods : prods pr",
	"prods : pr",
	"pr : r NEWE",
	"pr : error NEWE",
	"r : CHAR",
	"r : STR",
	"r : '.'",
	"r : CCL",
	"r : NCCL",
	"r : r '*'",
	"r : r '+'",
	"r : r '?'",
	"r : r '|' r",
	"r : r r",
	"r : r '/' r",
	"r : r ITER ',' ITER '}'",
	"r : r ITER '}'",
	"r : r ITER ',' '}'",
	"r : SCON r",
	"r : '^' r",
	"r : r '$'",
	"r : '(' r ')'",
	"r : NULLS",
};
#endif /* YYDEBUG */
/*	@(#)yaccpar	1.9	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	YYSTYPE *yypvt;	/* top of value stack for $vars */

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		YYSTYPE *yy_pv;	/* top of value stack */
		int *yy_ps;		/* top of state stack */
		int yy_state;		/* current state */
		int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				 int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 40 "parser.y"
{	
# ifdef DEBUG
		if(debug) sect2dump();
# endif
	} break;
case 3:
# line 48 "parser.y"
{
		if(!funcflag)phead2();
		funcflag = TRUE;
	} break;
case 4:
# line 53 "parser.y"
{
# ifdef DEBUG
		if(debug) {
			sect1dump();
			sect2dump();
			}
# endif
		} break;
case 7:
# line 64 "parser.y"
{	strcpy(dp, yypvt[-1].cp);
		def[dptr] = dp;
		dp += strlen(yypvt[-1].cp) + 1;
		strcpy(dp, yypvt[-0].cp);
		subs[dptr++] = dp;
		if(dptr >= DEFSIZE)
			error("Too many definitions");
		dp += slength(yypvt[-0]) + 1;
		if(dp >= dchar+DEFCHAR)
			error("Definitions too long");
		subs[dptr]=def[dptr]=0;	/* for lookup - require ending null */
	} break;
case 9:
# line 79 "parser.y"
{
# ifdef DEBUG
		if(sect == DEFSECTION && debug) sect1dump();
# endif
		sect++;
		} break;
case 10:
# line 87 "parser.y"
{	yyval.i = mn2(RNEWE,yypvt[-1].i,yypvt[-0].i);
		} break;
case 11:
# line 90 "parser.y"
{	yyval.i = yypvt[-0].i;} break;
case 12:
# line 93 "parser.y"
{
		if(divflg == TRUE)
			i = mn1(S1FINAL,casecount);
		else i = mn1(FINAL,casecount);
		yyval.i = mn2(RCAT,yypvt[-1].i,i);
		divflg = FALSE;
		casecount++;
		} break;
case 13:
# line 102 "parser.y"
{
# ifdef DEBUG
		if(debug) sect2dump();
# endif
		} break;
case 14:
# line 108 "parser.y"
{	yyval.i = mn0(yypvt[-0].i); } break;
case 15:
# line 110 "parser.y"
{
		p = yypvt[-0].cp;
		i = mn0(*p++);
		while(*p)
			i = mn2(RSTR,i,*p++);
		yyval.i = i;
		} break;
case 16:
# line 118 "parser.y"
{	symbol['\n'] = 0;
		if(psave == FALSE){
			p = ccptr;
			psave = ccptr;
			for(i=1;i<'\n';i++){
				symbol[i] = 1;
				*ccptr++ = i;
				}
			for(i='\n'+1;i<NCH;i++){
				symbol[i] = 1;
				*ccptr++ = i;
				}
			*ccptr++ = 0;
			if(ccptr > ccl+CCLSIZE)
				error("Too many large character classes");
			}
		else
			p = psave;
		yyval.i = mn1(RCCL,p);
		cclinter(1);
		} break;
case 17:
# line 140 "parser.y"
{	yyval.i = mn1(RCCL,yypvt[-0].i); } break;
case 18:
# line 142 "parser.y"
{	yyval.i = mn1(RNCCL,yypvt[-0].i); } break;
case 19:
# line 144 "parser.y"
{	yyval.i = mn1(STAR,yypvt[-1].i); } break;
case 20:
# line 146 "parser.y"
{	yyval.i = mn1(PLUS,yypvt[-1].i); } break;
case 21:
# line 148 "parser.y"
{	yyval.i = mn1(QUEST,yypvt[-1].i); } break;
case 22:
# line 150 "parser.y"
{	yyval.i = mn2(BAR,yypvt[-2].i,yypvt[-0].i); } break;
case 23:
# line 152 "parser.y"
{	yyval.i = mn2(RCAT,yypvt[-1].i,yypvt[-0].i); } break;
case 24:
# line 154 "parser.y"
{	if(!divflg){
			j = mn1(S2FINAL,-casecount);
			i = mn2(RCAT,yypvt[-2].i,j);
			yyval.i = mn2(DIV,i,yypvt[-0].i);
			}
		else {
			yyval.i = mn2(RCAT,yypvt[-2].i,yypvt[-0].i);
			warning("Extra slash removed");
			}
		divflg = TRUE;
		} break;
case 25:
# line 166 "parser.y"
{	if(yypvt[-3].i > yypvt[-1].i){
			i = yypvt[-3].i;
			yypvt[-3].i = yypvt[-1].i;
			yypvt[-1].i = i;
			}
		if(yypvt[-1].i <= 0)
			warning("Iteration range must be positive");
		else {
			j = yypvt[-4].i;
			for(k = 2; k<=yypvt[-3].i;k++)
				j = mn2(RCAT,j,dupl(yypvt[-4].i));
			for(i = yypvt[-3].i+1; i<=yypvt[-1].i; i++){
				g = dupl(yypvt[-4].i);
				for(k=2;k<=i;k++)
					g = mn2(RCAT,g,dupl(yypvt[-4].i));
				j = mn2(BAR,j,g);
				}
			yyval.i = j;
			}
	} break;
case 26:
# line 187 "parser.y"
{
		if(yypvt[-1].i < 0)warning("Can't have negative iteration");
		else if(yypvt[-1].i == 0) yyval.i = mn0(RNULLS);
		else {
			j = yypvt[-2].i;
			for(k=2;k<=yypvt[-1].i;k++)
				j = mn2(RCAT,j,dupl(yypvt[-2].i));
			yyval.i = j;
			}
		} break;
case 27:
# line 198 "parser.y"
{
				/* from n to infinity */
		if(yypvt[-2].i < 0)warning("Can't have negative iteration");
		else if(yypvt[-2].i == 0) yyval.i = mn1(STAR,yypvt[-3].i);
		else if(yypvt[-2].i == 1)yyval.i = mn1(PLUS,yypvt[-3].i);
		else {		/* >= 2 iterations minimum */
			j = yypvt[-3].i;
			for(k=2;k<yypvt[-2].i;k++)
				j = mn2(RCAT,j,dupl(yypvt[-3].i));
			k = mn1(PLUS,dupl(yypvt[-3].i));
			yyval.i = mn2(RCAT,j,k);
			}
		} break;
case 28:
# line 212 "parser.y"
{	yyval.i = mn2(RSCON,yypvt[-0].i,yypvt[-1].i); } break;
case 29:
# line 214 "parser.y"
{	yyval.i = mn1(CARAT,yypvt[-0].i); } break;
case 30:
# line 216 "parser.y"
{	i = mn0('\n');
		if(!divflg){
			j = mn1(S2FINAL,-casecount);
			k = mn2(RCAT,yypvt[-1].i,j);
			yyval.i = mn2(DIV,k,i);
			}
		else yyval.i = mn2(RCAT,yypvt[-1].i,i);
		divflg = TRUE;
		} break;
case 31:
# line 226 "parser.y"
{	yyval.i = yypvt[-1].i; } break;
case 32:
# line 228 "parser.y"
{	yyval.i = mn0(RNULLS); } break;
	}
	goto yystack;		/* reset registers in driver code */
}
