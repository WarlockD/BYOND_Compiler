#line 8 "btyaccpa.ske"

//
// @(#)btyaccpar, based on byacc 1.8 (Berkeley)
//
#define YYBTYACC 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Yshort;

#define number 257
#define stop 258
#define DEFINED 259
#define EQ 260
#define NE 261
#define LE 262
#define GE 263
#define LS 264
#define RS 265
#define ANDAND 266
#define OROR 267
#define UMINUS 268
static constexpr Yshort YYERRCODE = 256;
static constexpr Yshort yylhs[] = {                                        -1,
    0,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    2,    2,    2,    2,    2,    2,    2,
};
static constexpr Yshort yylen[] = {                                         2,
    2,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    5,
    3,    1,    2,    2,    2,    3,    4,    2,    1,
};
static constexpr Yshort yydefred[] = {                                      0,
   29,    0,    0,    0,    0,    0,    0,    0,   22,   28,
    0,   23,   24,   25,    0,    1,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   26,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    2,    3,    4,   27,    0,
    0,
};
static constexpr Yshort yydgoto[] = {                                       7,
    8,    9,
};
static constexpr Yshort yysindex[] = {                                    358,
    0,  -38,  358,  358,  358,  358,    0,  -37,    0,    0,
 -254,    0,    0,    0,  -26,    0,  358,  358,  358,  358,
  358,  358,  358,  358,  358,  358,  358,  358,  358,  358,
  358,  358,  358,  358,  358,  358,  -28,    0,  405,  405,
  130,  130,  139,  139,  367,  108,   13,    1,  277,  277,
  394,  130,  130,  -33,  -33,    0,    0,    0,    0,  358,
   13,
};
static constexpr Yshort yyrindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  321,  344,
   -9,   91,   28,   80,  -11,  429,   52,    0,  419,  425,
  355,  120,  159,   39,   68,    0,    0,    0,    0,    0,
   59,
};
static constexpr Yshort yycindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
static constexpr Yshort yygindex[] = {                                      0,
  538,   37,
};
static constexpr size_t YYTABLESIZE = 696
static constexpr Yshort yytable[] = {                                      36,
   29,   11,   37,   36,   34,   32,   25,   33,   34,   35,
   36,   29,   59,   35,   38,   34,   32,   25,   33,    0,
   35,    0,   30,    0,   31,   26,    0,    0,   11,   18,
    0,   11,   18,   30,   11,   31,   26,   36,   29,   12,
   13,   14,   34,   32,   25,   33,   18,   35,   11,   36,
   29,   18,    0,   11,   34,   32,   28,   33,   60,   35,
   30,    0,   31,   26,    0,    7,    0,   28,    7,    0,
    0,    7,   30,    0,   31,   26,    5,    0,    0,    5,
    0,    5,    5,    5,   11,    7,   27,    7,    0,    7,
    7,    0,   21,    0,   28,   21,    5,   27,    5,   20,
    5,    5,   20,    0,    0,    6,   28,    0,    6,   21,
    6,    6,    6,    0,   11,    0,   20,    8,    0,    0,
    8,    7,    0,    8,   27,    6,    0,    6,   12,    6,
    6,   12,    5,    0,   12,    0,   27,    8,    0,    8,
    0,    8,    8,    0,   36,   29,    0,    0,   12,   34,
   32,    7,   33,   12,   35,    0,    0,    9,    0,    0,
    9,    6,    5,    9,    0,    0,   36,   30,    0,   31,
    0,   34,   32,    8,   33,   36,   35,    9,    0,    0,
   34,   32,    9,   33,   12,   35,    0,    0,    0,    0,
    0,    6,    0,    0,    0,    0,   10,    0,    0,   10,
    0,   28,   10,    8,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    9,   12,    0,   10,    0,   10,    0,
   16,   10,   17,   18,   19,   20,   21,   22,   23,   24,
    0,   27,    0,   17,   18,   19,   20,   21,   22,   23,
   24,    0,    0,    9,    0,    0,   18,    0,   11,    0,
   11,   11,   10,    0,   18,   18,   11,   11,    0,    0,
   17,   18,   19,   20,   21,   22,   23,   24,    0,    0,
    0,    0,   17,   18,   19,   20,   21,   22,   23,   24,
    0,    0,   10,    0,    0,    7,    0,    7,    7,    7,
    7,    7,    7,    7,    7,    0,    5,    0,    5,    5,
    5,    5,    5,    5,    5,    5,    0,    0,    0,   21,
    0,    0,    0,   36,   29,    0,   20,    0,   34,   32,
    0,   33,    0,   35,    0,    6,    0,    6,    6,    6,
    6,    6,    6,    6,    6,    0,   30,    8,   31,    8,
    8,    8,    8,    8,    8,    8,    8,    0,   12,    0,
   12,   12,    0,    0,    0,    0,   12,   12,   13,    0,
    0,   13,    0,    0,   13,    0,    0,   17,   18,   19,
   20,   21,   22,   23,    0,    0,    0,    9,   13,    9,
    9,   14,    0,   13,   14,    9,    9,   14,    0,    0,
    4,    0,   15,   21,   22,   15,    0,    6,   15,    0,
    0,   14,    3,   36,   29,    0,   14,    0,   34,   32,
    0,   33,   15,   35,   13,    0,   10,   15,   10,   10,
    0,    0,    0,    0,   10,   10,   30,    0,   31,    0,
   36,    0,    0,    0,    0,   34,   32,   14,   33,    0,
   35,   36,    0,    0,   13,    0,   34,   32,   15,   33,
    0,   35,    0,   30,    0,   31,    0,    0,    0,   17,
   28,    0,   17,    0,   30,   16,   31,   14,   16,   19,
    0,    0,   19,    0,    0,    0,   17,    0,   15,    0,
    0,   17,   16,    5,    0,    0,   19,   16,    0,    0,
   27,   19,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   17,    0,    0,    0,    0,    0,   16,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   17,   18,   19,   20,
   21,   22,   17,   15,    0,    0,    0,    0,   16,    0,
    0,    0,    0,    0,   39,   40,   41,   42,   43,   44,
   45,   46,   47,   48,   49,   50,   51,   52,   53,   54,
   55,   56,   57,   58,    0,    0,    0,    0,   13,    0,
    0,    0,    0,    0,    0,    0,   13,   13,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   61,    0,    0,
    0,   14,    0,    0,    0,    0,    0,    0,    0,   14,
   14,    0,   15,    0,    1,    0,    2,    0,    0,    0,
   15,   15,    0,    0,    0,    0,   17,   18,   19,   20,
   21,   22,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   17,   18,   19,   20,   21,   22,    0,
    0,    0,    0,    0,    0,    0,   19,   20,   21,   22,
    0,    0,    0,    0,    0,    0,   17,    0,    0,    0,
    0,    0,   16,    0,   17,   17,   19,    0,    0,    0,
   16,   16,    0,    0,    0,   19,
};
static constexpr Yshort yycheck[] = {                                      37,
   38,   40,  257,   37,   42,   43,   44,   45,   42,   47,
   37,   38,   41,   47,   41,   42,   43,   44,   45,   -1,
   47,   -1,   60,   -1,   62,   63,   -1,   -1,   38,   41,
   -1,   41,   44,   60,   44,   62,   63,   37,   38,    3,
    4,    5,   42,   43,   44,   45,   58,   47,   58,   37,
   38,   63,   -1,   63,   42,   43,   94,   45,   58,   47,
   60,   -1,   62,   63,   -1,   38,   -1,   94,   41,   -1,
   -1,   44,   60,   -1,   62,   63,   38,   -1,   -1,   41,
   -1,   43,   44,   45,   94,   58,  124,   60,   -1,   62,
   63,   -1,   41,   -1,   94,   44,   58,  124,   60,   41,
   62,   63,   44,   -1,   -1,   38,   94,   -1,   41,   58,
   43,   44,   45,   -1,  124,   -1,   58,   38,   -1,   -1,
   41,   94,   -1,   44,  124,   58,   -1,   60,   38,   62,
   63,   41,   94,   -1,   44,   -1,  124,   58,   -1,   60,
   -1,   62,   63,   -1,   37,   38,   -1,   -1,   58,   42,
   43,  124,   45,   63,   47,   -1,   -1,   38,   -1,   -1,
   41,   94,  124,   44,   -1,   -1,   37,   60,   -1,   62,
   -1,   42,   43,   94,   45,   37,   47,   58,   -1,   -1,
   42,   43,   63,   45,   94,   47,   -1,   -1,   -1,   -1,
   -1,  124,   -1,   -1,   -1,   -1,   38,   -1,   -1,   41,
   -1,   94,   44,  124,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   94,  124,   -1,   58,   -1,  257,   -1,
  258,   63,  260,  261,  262,  263,  264,  265,  266,  267,
   -1,  124,   -1,  260,  261,  262,  263,  264,  265,  266,
  267,   -1,   -1,  124,   -1,   -1,  258,   -1,  258,   -1,
  260,  261,   94,   -1,  266,  267,  266,  267,   -1,   -1,
  260,  261,  262,  263,  264,  265,  266,  267,   -1,   -1,
   -1,   -1,  260,  261,  262,  263,  264,  265,  266,  267,
   -1,   -1,  124,   -1,   -1,  258,   -1,  260,  261,  262,
  263,  264,  265,  266,  267,   -1,  258,   -1,  260,  261,
  262,  263,  264,  265,  266,  267,   -1,   -1,   -1,  258,
   -1,   -1,   -1,   37,   38,   -1,  258,   -1,   42,   43,
   -1,   45,   -1,   47,   -1,  258,   -1,  260,  261,  262,
  263,  264,  265,  266,  267,   -1,   60,  258,   62,  260,
  261,  262,  263,  264,  265,  266,  267,   -1,  258,   -1,
  260,  261,   -1,   -1,   -1,   -1,  266,  267,   38,   -1,
   -1,   41,   -1,   -1,   44,   -1,   -1,  260,  261,  262,
  263,  264,  265,  266,   -1,   -1,   -1,  258,   58,  260,
  261,   38,   -1,   63,   41,  266,  267,   44,   -1,   -1,
   33,   -1,   38,  264,  265,   41,   -1,   40,   44,   -1,
   -1,   58,   45,   37,   38,   -1,   63,   -1,   42,   43,
   -1,   45,   58,   47,   94,   -1,  258,   63,  260,  261,
   -1,   -1,   -1,   -1,  266,  267,   60,   -1,   62,   -1,
   37,   -1,   -1,   -1,   -1,   42,   43,   94,   45,   -1,
   47,   37,   -1,   -1,  124,   -1,   42,   43,   94,   45,
   -1,   47,   -1,   60,   -1,   62,   -1,   -1,   -1,   41,
   94,   -1,   44,   -1,   60,   41,   62,  124,   44,   41,
   -1,   -1,   44,   -1,   -1,   -1,   58,   -1,  124,   -1,
   -1,   63,   58,  126,   -1,   -1,   58,   63,   -1,   -1,
  124,   63,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   94,   -1,   -1,   -1,   -1,   -1,   94,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  260,  261,  262,  263,
  264,  265,  124,    6,   -1,   -1,   -1,   -1,  124,   -1,
   -1,   -1,   -1,   -1,   17,   18,   19,   20,   21,   22,
   23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
   33,   34,   35,   36,   -1,   -1,   -1,   -1,  258,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  266,  267,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   60,   -1,   -1,
   -1,  258,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  266,
  267,   -1,  258,   -1,  257,   -1,  259,   -1,   -1,   -1,
  266,  267,   -1,   -1,   -1,   -1,  260,  261,  262,  263,
  264,  265,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  260,  261,  262,  263,  264,  265,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  262,  263,  264,  265,
   -1,   -1,   -1,   -1,   -1,   -1,  258,   -1,   -1,   -1,
   -1,   -1,  258,   -1,  266,  267,  258,   -1,   -1,   -1,
  266,  267,   -1,   -1,   -1,  267,
};
static constexpr Yshort yyctable[] = {                                      0,
};
static constexpr Yshort YYFINAL = 7;
#ifndef YYDEBUG
static constexpr Yshort YYDEBUG = 0;
#endif
static constexpr Yshort YYMAXTOKEN =268;
#if YYDEBUG
static constexpr const char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,
0,0,0,0,0,0,0,"':'",0,"'<'","'='","'>'","'?'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,"'|'",0,"'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"number","stop","DEFINED","EQ","NE",
"LE","GE","LS","RS","ANDAND","OROR","UMINUS",
};
static constexpr char *yyrule[] = {
"$accept : S",
"S : e stop",
"e : e '*' e",
"e : e '/' e",
"e : e '%' e",
"e : e '+' e",
"e : e '-' e",
"e : e LS e",
"e : e RS e",
"e : e '<' e",
"e : e '>' e",
"e : e LE e",
"e : e GE e",
"e : e EQ e",
"e : e NE e",
"e : e '&' e",
"e : e '^' e",
"e : e '|' e",
"e : e ANDAND e",
"e : e OROR e",
"e : e '?' e ':' e",
"e : e ',' e",
"e : term",
"term : '-' term",
"term : '!' term",
"term : '~' term",
"term : '(' e ')'",
"term : DEFINED '(' number ')'",
"term : DEFINED number",
"term : number",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#line 42 "btyaccpa.ske"

//
// YYPOSN is user-defined text position type.
//
#ifndef YYPOSN
#define YYPOSN int
#endif

#ifdef YYREDUCEPOSNFUNC
#define YYCALLREDUCEPOSN(e)   \
	if(reduce_posn) {     \
	  YYREDUCEPOSNFUNC(yyps->pos, &(yyps->psp)[1-yym], &(yyps->vsp)[1-yym], \
			   yym, yyps->psp - yyps->ps, yychar, yyposn, e);       \
	  reduce_posn = 0;    \
	}

#ifndef YYCALLREDUCEPOSNARG
#define YYCALLREDUCEPOSNARG yyps->val
#endif


#define YYPOSNARG(n) ((yyps->psp)[1-yym+(n)-1])
#define YYPOSNOUT    (yyps->pos)
#endif

// If delete function is not defined by the user, do not deletions.
#ifndef YYDELETEVAL
#define YYDELETEVAL(v) 
#endif

// If delete function is not defined by the user, do not deletions.
#ifndef YYDELETEPOSN
#define YYDELETEPOSN(v) 
#endif

#define yyclearin (yychar=(-1))

#define yyerrok (yyps->errflag=0)

#ifndef YYSTACKGROWTH
#define YYSTACKGROWTH 16
#endif

#ifndef YYDEFSTACKSIZE
#define YYDEFSTACKSIZE 12
#endif

#ifdef YYDEBUG
int yydebug;
#endif

int yynerrs;

/* These value/posn are taken from the lexer */
YYSTYPE yylval;
YYPOSN  yyposn;

/* These value/posn of the root non-terminal are returned to the caller */
YYSTYPE yyretlval;
YYPOSN  yyretposn;

#define YYABORT  goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#define YYVALID         do { if (yyps->save)          goto yyvalid; } while(0)
#define YYVALID_NESTED  do { if (yyps->save && \
                                 yyps->save->save==0) goto yyvalid; } while(0)

struct yyparsestate {
  yyparsestate *save;        // Previously saved parser state
  int           state;
  int           errflag;
  Yshort       *ssp;         // state stack pointer
  YYSTYPE      *vsp;         // value stack pointer
  YYPOSN       *psp;         // position stack pointer
  YYSTYPE       val;         // value as returned by actions
  YYPOSN        pos;         // position as returned by universal action
  Yshort       *ss;          // state stack base
  YYSTYPE      *vs;          // values stack base
  YYPOSN       *ps;          // position stack base
  int           lexeme;      // index of the conflict lexeme in the lexical queue
  unsigned int  stacksize;   // current maximum stack size
  Yshort        ctry;        // index in yyctable[] for this conflict
};

// Current parser state
static yyparsestate *yyps=0;

// yypath!=NULL: do the full parse, starting at *yypath parser state.
static yyparsestate *yypath=0;

// Base of the lexical value queue
static YYSTYPE *yylvals=0;

// Current posistion at lexical value queue
static YYSTYPE *yylvp=0;

// End position of lexical value queue
static YYSTYPE *yylve=0;

// The last allocated position at the lexical value queue
static YYSTYPE *yylvlim=0;

// Base of the lexical position queue
static YYPOSN *yylpsns=0;

// Current posistion at lexical position queue
static YYPOSN *yylpp=0;

// End position of lexical position queue
static YYPOSN *yylpe=0;

// The last allocated position at the lexical position queue
static YYPOSN *yylplim=0;

// Current position at lexical token queue
static Yshort *yylexp=0;

static Yshort *yylexemes=0;

//
// For use in generated program
//
#define yytrial (yyps->save)
#define yyvsp   (yyps->vsp)
#define yyval   (yyps->val)
#define yydepth (yyps->ssp - yyps->ss)


//
// Local prototypes.
//
int yyparse(void);

int  YYLex1();
int  yyexpand();
void YYSCopy(YYSTYPE *to, YYSTYPE *from, int size);
void YYPCopy(YYPOSN  *to, YYPOSN  *from, int size);
void YYMoreStack(yyparsestate *yyps);
yyparsestate *YYNewState(int size);
void YYFreeState(yyparsestate *p);


#line 92 "preprocessor.y"

#line 426 "y_tab.c"
#line 186 "btyaccpa.ske"


//
// Parser function
//
int yyparse() {
  int yym, yyn, yystate, yychar, yynewerrflag;
  yyparsestate *yyerrctx = NULL;
  int reduce_posn;

#if YYDEBUG
  char *yys;
  
  if ((yys = getenv("YYDEBUG"))) {
    yyn = *yys;
    if (yyn >= '0' && yyn <= '9')
      yydebug = yyn - '0'; 
  }
#endif
  
  yyps = YYNewState(YYDEFSTACKSIZE);
  yyps->save = 0;
  yynerrs = 0;
  yyps->errflag = 0;
  yychar = (-1);
  
  yyps->ssp = yyps->ss;
  yyps->vsp = yyps->vs;
  yyps->psp = yyps->ps;
  *(yyps->ssp) = yystate = 0;
  

  //
  // Main parsing loop
  //
 yyloop:
  if ((yyn = yydefred[yystate])) {
    goto yyreduce;
  }

  //
  // Read one token
  //
  if (yychar < 0) {
    if ((yychar = YYLex1()) < 0) yychar = 0;
#if YYDEBUG
    if (yydebug) {
      yys = 0;
      if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
      if (!yys) yys = "illegal-symbol";
      printf("yydebug[%d,%d]: state %d, reading %d (%s)", 
	     yydepth, (int)yytrial, yystate, yychar, yys);
#ifdef YYDBPR
      printf("<");
      YYDBPR(yylval);
      printf(">");
#endif
      printf("\n"); 
    }
#endif
  }

  //
  // Do we have a conflict?
  //
  if ((yyn = yycindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar) {
    int ctry;

    if (yypath) {
#if YYDEBUG
      if (yydebug) {
        printf("yydebug[%d,%d]: CONFLICT in state %d: following successful trial parse\n", 
	       yydepth, (int)yytrial, yystate);
      }
#endif
      // Switch to the next conflict context
      yyparsestate *save = yypath;
      yypath = save->save;
      ctry = save->ctry;
      if (save->state != yystate) 
        goto yyabort;
      YYFreeState(save); 

    } else {

#if YYDEBUG
      if (yydebug) {
        printf("yydebug[%d,%d]: CONFLICT in state %d. ", 
	       yydepth, (int)yytrial, yystate);
        if(yyps->save) {
          printf("ALREADY in conflict. Continue trial parse.");
        } else {
          printf("Start trial parse.");
        }
        printf("\n");
      }
#endif
      yyparsestate *save = YYNewState(yyps->ssp - yyps->ss);
      save->save    = yyps->save;
      save->state   = yystate;
      save->errflag = yyps->errflag;
      save->ssp     = save->ss + (yyps->ssp - yyps->ss);
      save->vsp     = save->vs + (yyps->vsp - yyps->vs);
      save->psp     = save->ps + (yyps->psp - yyps->ps);
      memcpy (save->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));
      YYSCopy(save->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));
      YYPCopy(save->ps, yyps->ps, (yyps->ssp - yyps->ss + 1));
      ctry = yytable[yyn];
      if (yyctable[ctry] == -1) {
#if YYDEBUG
        if (yydebug && yychar >= 0)
          printf("yydebug[%d]: backtracking 1 token\n", 
		 (int)yytrial);
#endif
        ctry++; 
      }
      save->ctry = ctry;
      if (!yyps->save) {
        // If this is a first conflict in the stack, start saving lexemes
        if (!yylexemes) {
          yylexemes = new Yshort[YYSTACKGROWTH];
          yylvals = new YYSTYPE[YYSTACKGROWTH];
          yylvlim = yylvals + YYSTACKGROWTH; 
          yylpsns = new YYPOSN[YYSTACKGROWTH];
          yylplim = yylpsns + YYSTACKGROWTH; 
        }
        if (yylvp == yylve) {
          yylvp = yylve = yylvals;
	  yylpp = yylpe = yylpsns;
          yylexp = yylexemes;
          if (yychar >= 0) {
            *yylve++ = yylval;
            *yylpe++ = yyposn;
            *yylexp = yychar;
            yychar = -1; 
          } 
        } 
      }
      if (yychar >= 0) {
        yylvp--, yylpp--, yylexp--;
        yychar = -1; 
      }
      save->lexeme = yylvp - yylvals;
      yyps->save = save; 
    }
    if (yytable[yyn] == ctry) {
#if YYDEBUG
      if (yydebug)
        printf("yydebug[%d,%d]: state %d, shifting to state %d\n",
               yydepth, (int)yytrial, yystate, yyctable[ctry]);
#endif
      if (yychar < 0)
        yylvp++, yylpp++, yylexp++;
      yychar = -1;
      if (yyps->errflag > 0) --yyps->errflag;
      yystate = yyctable[ctry];
      goto yyshift; 
    } else {
      yyn = yyctable[ctry];
      goto yyreduce; 
    } 
  }

  //
  // Is action a shift?
  //
  if ((yyn = yysindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar) {
#if YYDEBUG
    if (yydebug)
      printf("yydebug[%d,%d]: state %d, shifting to state %d\n",
             yydepth, (int)yytrial, yystate, yytable[yyn]);
#endif
    yychar = (-1);
    if (yyps->errflag > 0)  --yyps->errflag;
    yystate = yytable[yyn];
  yyshift:
    if (yyps->ssp >= yyps->ss + yyps->stacksize - 1) {
      YYMoreStack(yyps);
    }
    *++(yyps->ssp) = yystate;
    *++(yyps->vsp) = yylval;
    *++(yyps->psp) = yyposn;
    goto yyloop;
  }
  if ((yyn = yyrindex[yystate]) &&
      (yyn += yychar) >= 0 &&
      yyn <= YYTABLESIZE &&
      yycheck[yyn] == yychar) {
    yyn = yytable[yyn];
    goto yyreduce;
  }

  //
  // Action: error
  //
  if (yyps->errflag) goto yyinrecovery;
  yynewerrflag = 1;
  goto yyerrhandler;
yyerrlab:
  yynewerrflag = 0;
yyerrhandler:
  while (yyps->save) { 
    int ctry; 
    yyparsestate *save = yyps->save;
#if YYDEBUG
    if (yydebug)
      printf("yydebug[%d,%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
             yydepth, (int)yytrial, yystate, yyps->save->state, yylvp - yylvals - yyps->save->lexeme);
#endif
    // Memorize most forward-looking error state in case
    // it's really an error.
    if(yyerrctx==NULL || yyerrctx->lexeme<yylvp-yylvals) {
      // Free old saved error context state
      if(yyerrctx) YYFreeState(yyerrctx);
      // Create and fill out new saved error context state
      yyerrctx = YYNewState(yyps->ssp - yyps->ss);
      yyerrctx->save = yyps->save;
      yyerrctx->state = yystate;
      yyerrctx->errflag = yyps->errflag;
      yyerrctx->ssp = yyerrctx->ss + (yyps->ssp - yyps->ss);
      yyerrctx->vsp = yyerrctx->vs + (yyps->vsp - yyps->vs);
      yyerrctx->psp = yyerrctx->ps + (yyps->psp - yyps->ps);
      memcpy (yyerrctx->ss, yyps->ss, (yyps->ssp - yyps->ss + 1)*sizeof(Yshort));
      YYSCopy(yyerrctx->vs, yyps->vs, (yyps->ssp - yyps->ss + 1));
      YYPCopy(yyerrctx->ps, yyps->ps, (yyps->ssp - yyps->ss + 1));
      yyerrctx->lexeme = yylvp - yylvals;
    }
    yylvp  = yylvals   + save->lexeme;
    yylpp  = yylpsns   + save->lexeme;
    yylexp = yylexemes + save->lexeme;
    yychar = -1;
    yyps->ssp = yyps->ss + (save->ssp - save->ss);
    yyps->vsp = yyps->vs + (save->vsp - save->vs);
    yyps->psp = yyps->ps + (save->psp - save->ps);
    memcpy (yyps->ss, save->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
    YYSCopy(yyps->vs, save->vs,  yyps->vsp - yyps->vs + 1);
    YYPCopy(yyps->ps, save->ps,  yyps->psp - yyps->ps + 1);
    ctry = ++save->ctry;
    yystate = save->state;
    // We tried shift, try reduce now
    if ((yyn = yyctable[ctry]) >= 0) {
      goto yyreduce;
    }
    yyps->save = save->save;
    YYFreeState(save);
    //
    // Nothing left on the stack -- error
    //
    if (!yyps->save) {
#if YYDEBUG
      if (yydebug) {
        printf("yydebug[%d]: trial parse FAILED, entering ERROR mode\n", 
	       (int)yytrial);
      }
#endif
      // Restore state as it was in the most forward-advanced error
      yylvp  = yylvals   + yyerrctx->lexeme;
      yylpp  = yylpsns   + yyerrctx->lexeme;
      yylexp = yylexemes + yyerrctx->lexeme;
      yychar = yylexp[-1];
      yylval = yylvp[-1];
      yyposn = yylpp[-1];
      yyps->ssp = yyps->ss + (yyerrctx->ssp - yyerrctx->ss);
      yyps->vsp = yyps->vs + (yyerrctx->vsp - yyerrctx->vs);
      yyps->psp = yyps->ps + (yyerrctx->psp - yyerrctx->ps);
      memcpy (yyps->ss, yyerrctx->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
      YYSCopy(yyps->vs, yyerrctx->vs,  yyps->vsp - yyps->vs + 1);
      YYPCopy(yyps->ps, yyerrctx->ps,  yyps->psp - yyps->ps + 1);
      yystate = yyerrctx->state;
      YYFreeState(yyerrctx);
      yyerrctx = NULL;
    }
    yynewerrflag = 1; 
  }
  if (yynewerrflag) {
#ifdef YYERROR_DETAILED
    yyerror_detailed("syntax error", yychar, yylval, yyposn);
#else
    yyerror("syntax error");
#endif
  }
  ++yynerrs;
 yyinrecovery:
  if (yyps->errflag < 3) {
    yyps->errflag = 3;
    for (;;) {
      if ((yyn = yysindex[*(yyps->ssp)]) && 
	  (yyn += YYERRCODE) >= 0 &&
          yyn <= YYTABLESIZE && 
	  yycheck[yyn] == YYERRCODE) {
#if YYDEBUG
        if (yydebug)
          printf("yydebug[%d,%d]: state %d, ERROR recovery shifts to state %d\n",
                 yydepth, (int)yytrial, *(yyps->ssp), yytable[yyn]);
#endif
        /* Use label yyerrlab, so that compiler does not warn */
        if(yyps->errflag != yyps->errflag) goto yyerrlab;
        yystate = yytable[yyn];
        goto yyshift; 
      } else {
#if YYDEBUG
        if (yydebug)
          printf("yydebug[%d,%d]: ERROR recovery discards state %d\n",
                 yydepth, (int)yytrial, *(yyps->ssp));
#endif
        if (yyps->ssp <= yyps->ss) {
	  goto yyabort;
	}
	if(!yytrial) {
	  YYDELETEVAL(yyps->vsp[0],1);
	  YYDELETEPOSN(yyps->psp[0],1);
	}
        --(yyps->ssp);
        --(yyps->vsp);
        --(yyps->psp);
      }
    }
  } else {
    if (yychar == 0) goto yyabort;
#if YYDEBUG
    if (yydebug) {
      yys = 0;
      if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
      if (!yys) yys = "illegal-symbol";
      printf("yydebug[%d,%d]: state %d, ERROR recovery discards token %d (%s)\n",
             yydepth, (int)yytrial, yystate, yychar, yys); 
    }
#endif
    if(!yytrial) {
      YYDELETEVAL(yylval,0);
      YYDELETEPOSN(yyposn,0);
    }
    yychar = (-1);
    goto yyloop;
  }

  //
  // Reduce the rule
  //
yyreduce:
  yym = yylen[yyn];
#if YYDEBUG
  if (yydebug) {
    printf("yydebug[%d,%d]: state %d, reducing by rule %d (%s)",
           yydepth, (int)yytrial, yystate, yyn, yyrule[yyn]);
#ifdef YYDBPR
    if (yym) {
      int i;
      printf("<");
      for (i=yym; i>0; i--) {
        if (i!=yym) printf(", ");
        YYDBPR((yyps->vsp)[1-i]);
      }
      printf(">");
    }
#endif
    printf("\n");
  }
#endif
  if (yyps->ssp + 1 - yym >= yyps->ss + yyps->stacksize) {
    YYMoreStack(yyps);
  }

  /* "$$ = NULL" default action */
  memset(&yyps->val, 0, sizeof(yyps->val));

  /* default reduced position is NULL -- no position at all.
     no position will be assigned at trial time and if no position handling is present */
  memset(&yyps->pos, 0, sizeof(yyps->pos));

  reduce_posn = TRUE;

  switch (yyn) {

case 1:
  if (!yytrial)
#line 31 "preprocessor.y"
{return(yyvsp[-1]);}
#line 812 "y_tab.c"
break;
case 2:
  if (!yytrial)
#line 35 "preprocessor.y"
{yyval = yyvsp[-2] * yyvsp[0];}
#line 818 "y_tab.c"
break;
case 3:
  if (!yytrial)
#line 37 "preprocessor.y"
{yyval = yyvsp[-2] / yyvsp[0];}
#line 824 "y_tab.c"
break;
case 4:
  if (!yytrial)
#line 39 "preprocessor.y"
{yyval = yyvsp[-2] % yyvsp[0];}
#line 830 "y_tab.c"
break;
case 5:
  if (!yytrial)
#line 41 "preprocessor.y"
{yyval = yyvsp[-2] + yyvsp[0];}
#line 836 "y_tab.c"
break;
case 6:
  if (!yytrial)
#line 43 "preprocessor.y"
{yyval = yyvsp[-2] - yyvsp[0];}
#line 842 "y_tab.c"
break;
case 7:
  if (!yytrial)
#line 45 "preprocessor.y"
{yyval = yyvsp[-2] << yyvsp[0];}
#line 848 "y_tab.c"
break;
case 8:
  if (!yytrial)
#line 47 "preprocessor.y"
{yyval = yyvsp[-2] >> yyvsp[0];}
#line 854 "y_tab.c"
break;
case 9:
  if (!yytrial)
#line 49 "preprocessor.y"
{yyval = yyvsp[-2] < yyvsp[0];}
#line 860 "y_tab.c"
break;
case 10:
  if (!yytrial)
#line 51 "preprocessor.y"
{yyval = yyvsp[-2] > yyvsp[0];}
#line 866 "y_tab.c"
break;
case 11:
  if (!yytrial)
#line 53 "preprocessor.y"
{yyval = yyvsp[-2] <= yyvsp[0];}
#line 872 "y_tab.c"
break;
case 12:
  if (!yytrial)
#line 55 "preprocessor.y"
{yyval = yyvsp[-2] >= yyvsp[0];}
#line 878 "y_tab.c"
break;
case 13:
  if (!yytrial)
#line 57 "preprocessor.y"
{yyval = yyvsp[-2] == yyvsp[0];}
#line 884 "y_tab.c"
break;
case 14:
  if (!yytrial)
#line 59 "preprocessor.y"
{yyval = yyvsp[-2] != yyvsp[0];}
#line 890 "y_tab.c"
break;
case 15:
  if (!yytrial)
#line 61 "preprocessor.y"
{yyval = yyvsp[-2] & yyvsp[0];}
#line 896 "y_tab.c"
break;
case 16:
  if (!yytrial)
#line 63 "preprocessor.y"
{yyval = yyvsp[-2] ^ yyvsp[0];}
#line 902 "y_tab.c"
break;
case 17:
  if (!yytrial)
#line 65 "preprocessor.y"
{yyval = yyvsp[-2] | yyvsp[0];}
#line 908 "y_tab.c"
break;
case 18:
  if (!yytrial)
#line 67 "preprocessor.y"
{yyval = yyvsp[-2] && yyvsp[0];}
#line 914 "y_tab.c"
break;
case 19:
  if (!yytrial)
#line 69 "preprocessor.y"
{yyval = yyvsp[-2] || yyvsp[0];}
#line 920 "y_tab.c"
break;
case 20:
  if (!yytrial)
#line 71 "preprocessor.y"
{yyval = yyvsp[-4] ? yyvsp[-2] : yyvsp[0];}
#line 926 "y_tab.c"
break;
case 21:
  if (!yytrial)
#line 73 "preprocessor.y"
{yyval = yyvsp[0];}
#line 932 "y_tab.c"
break;
case 22:
  if (!yytrial)
#line 75 "preprocessor.y"
{yyval = yyvsp[0];}
#line 938 "y_tab.c"
break;
case 23:
  if (!yytrial)
#line 78 "preprocessor.y"
{yyval = -yyvsp[0];}
#line 944 "y_tab.c"
break;
case 24:
  if (!yytrial)
#line 80 "preprocessor.y"
{yyval = !yyvsp[0];}
#line 950 "y_tab.c"
break;
case 25:
  if (!yytrial)
#line 82 "preprocessor.y"
{yyval = ~yyvsp[0];}
#line 956 "y_tab.c"
break;
case 26:
  if (!yytrial)
#line 84 "preprocessor.y"
{yyval = yyvsp[-1];}
#line 962 "y_tab.c"
break;
case 27:
  if (!yytrial)
#line 86 "preprocessor.y"
{yyval= yyvsp[-1];}
#line 968 "y_tab.c"
break;
case 28:
  if (!yytrial)
#line 88 "preprocessor.y"
{yyval = yyvsp[0];}
#line 974 "y_tab.c"
break;
case 29:
  if (!yytrial)
#line 90 "preprocessor.y"
{yyval= yyvsp[0];}
#line 980 "y_tab.c"
break;
#line 982 "y_tab.c"
#line 567 "btyaccpa.ske"

  default:
    break;
  }

#if YYDEBUG && defined(YYDBPR)
  if (yydebug) {
    printf("yydebug[%d]: after reduction, result is ", yytrial);
    YYDBPR(yyps->val);
    printf("\n");
  }
#endif

  // Perform user-defined position reduction
#ifdef YYREDUCEPOSNFUNC
  if(!yytrial) {
    YYCALLREDUCEPOSN(YYREDUCEPOSNFUNCARG);
  }
#endif

  yyps->ssp -= yym;
  yystate = *(yyps->ssp);
  yyps->vsp -= yym;
  yyps->psp -= yym;

  yym = yylhs[yyn];
  if (yystate == 0 && yym == 0) {
#if YYDEBUG
    if (yydebug) {
      printf("yydebug[%d,%d]: after reduction, shifting from state 0 to state %d\n", 
	     yydepth, (int)yytrial, YYFINAL);
    }
#endif
    yystate = YYFINAL;
    *++(yyps->ssp) = YYFINAL;
    *++(yyps->vsp) = yyps->val;
    yyretlval = yyps->val;	// return value of root non-terminal to yylval
    *++(yyps->psp) = yyps->pos;
    yyretposn = yyps->pos;	// return value of root position to yyposn
    if (yychar < 0) {
      if ((yychar = YYLex1()) < 0) {
        yychar = 0;
      }
#if YYDEBUG
      if (yydebug) {
        yys = 0;
        if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
        if (!yys) yys = "illegal-symbol";
        printf("yydebug[%d,%d]: state %d, reading %d (%s)\n", 
	       yydepth, (int)yytrial, YYFINAL, yychar, yys); 
      }
#endif
    }
    if (yychar == 0) goto yyaccept;
    goto yyloop;
  }

  if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
      yyn <= YYTABLESIZE && yycheck[yyn] == yystate) {
    yystate = yytable[yyn];
  } else {
    yystate = yydgoto[yym];
  }
#if YYDEBUG
  if (yydebug)
    printf("yydebug[%d,%d]: after reduction, shifting from state %d to state %d\n",
           yydepth, (int)yytrial, *(yyps->ssp), yystate);
#endif
  if (yyps->ssp >= yyps->ss + yyps->stacksize - 1) {
    YYMoreStack(yyps);
  }
  *++(yyps->ssp) = yystate;
  *++(yyps->vsp) = yyps->val;
  *++(yyps->psp) = yyps->pos;
  goto yyloop;


  //
  // Reduction declares that this path is valid.
  // Set yypath and do a full parse
  //
yyvalid:
  if (yypath) {
    goto yyabort;
  }
  while (yyps->save) {
    yyparsestate *save = yyps->save;
    yyps->save = save->save;
    save->save = yypath;
    yypath = save;
  }
#if YYDEBUG
  if (yydebug)
    printf("yydebug[%d,%d]: CONFLICT trial successful, backtracking to state %d, %d tokens\n",
           yydepth, (int)yytrial, yypath->state, yylvp - yylvals - yypath->lexeme);
#endif
  if(yyerrctx) {
    YYFreeState(yyerrctx); yyerrctx = NULL;
  }
  yychar = -1;
  yyps->ssp = yyps->ss + (yypath->ssp - yypath->ss);
  yyps->vsp = yyps->vs + (yypath->vsp - yypath->vs);
  yyps->psp = yyps->ps + (yypath->psp - yypath->ps);
  memcpy (yyps->ss, yypath->ss, (yyps->ssp - yyps->ss + 1) * sizeof(Yshort));
  YYSCopy(yyps->vs, yypath->vs,  yyps->vsp - yyps->vs + 1);
  YYPCopy(yyps->ps, yypath->ps,  yyps->psp - yyps->ps + 1);
  yylvp = yylvals + yypath->lexeme;
  yylpp = yylpsns + yypath->lexeme;
  yylexp = yylexemes + yypath->lexeme;
  yystate = yypath->state;
  goto yyloop;


yyabort:
  if(yyerrctx) {
    YYFreeState(yyerrctx); yyerrctx = NULL;
  }

  YYSTYPE *pv;
  for(pv=yyps->vs; pv<yyps->vsp; pv++) {
    YYDELETEVAL(*pv,2);
  }

  YYPOSN *pp;
  for(pp=yyps->ps; pp<yyps->psp; pp++) {
    YYDELETEPOSN(*pp,2);
  }

  while (yyps) {
    yyparsestate *save = yyps;
    yyps = save->save;
    YYFreeState(save);
  }
  while (yypath) {
    yyparsestate *save = yypath;
    yypath = save->save;
    YYFreeState(save); 
  }
  return (1);


yyaccept:
  if (yyps->save) goto yyvalid;
  if(yyerrctx) {
    YYFreeState(yyerrctx); yyerrctx = NULL;
  }
  while (yyps) {
    yyparsestate *save = yyps;
    yyps = save->save;
    YYFreeState(save);
  }
  while (yypath) {
    yyparsestate *save = yypath;
    yypath = save->save;
    YYFreeState(save); 
  }
  return (0);
}


int YYLex1() {
  if(yylvp<yylve) {
    yylval = *yylvp++;
    yyposn = *yylpp++;
    return *yylexp++;
  } else {
    if(yyps->save) {
      if(yylvp==yylvlim) {
	yyexpand();
      }
      *yylexp = yylex();
      *yylvp++ = yylval;
      yylve++;
      *yylpp++ = yyposn;
      yylpe++;
      return *yylexp++;
    } else {
      return yylex();
    }
  }
}

int yyexpand() {
  int p = yylvp-yylvals;
  int s = yylvlim-yylvals;
  s += YYSTACKGROWTH;
  { Yshort  *tl = yylexemes; 
    YYSTYPE *tv = yylvals;
    YYPOSN  *tp = yylpsns;
    yylvals = new YYSTYPE[s];
    yylpsns = new YYPOSN[s];
    yylexemes = new Yshort[s];
    memcpy(yylexemes, tl, (s-YYSTACKGROWTH)*sizeof(Yshort));
    YYSCopy(yylvals, tv, s-YYSTACKGROWTH);
    YYPCopy(yylpsns, tp, s-YYSTACKGROWTH);
    delete[] tl;
    delete[] tv;
    delete[] tp;
  }
  yylvp = yylve = yylvals + p;
  yylvlim = yylvals + s;
  yylpp = yylpe = yylpsns + p;
  yylplim = yylpsns + s;
  yylexp = yylexemes + p;
  return 0;
}

void YYSCopy(YYSTYPE *to, YYSTYPE *from, int size) {
  int i;                             
  for (i = size-1; i >= 0; i--) {
    to[i] = from[i];
  }
}

void YYPCopy(YYPOSN *to, YYPOSN *from, int size) {
  int i;                             
  for (i = size-1; i >= 0; i--) {
    to[i] = from[i];
  }
}

void YYMoreStack(yyparsestate *yyps) {
  int p = yyps->ssp - yyps->ss;                               
  Yshort  *tss = yyps->ss;
  YYSTYPE *tvs = yyps->vs;
  YYPOSN  *tps = yyps->ps;
  yyps->ss = new Yshort [yyps->stacksize + YYSTACKGROWTH];   
  yyps->vs = new YYSTYPE[yyps->stacksize + YYSTACKGROWTH];  
  yyps->ps = new YYPOSN [yyps->stacksize + YYSTACKGROWTH];  
  memcpy(yyps->ss, tss, yyps->stacksize * sizeof(Yshort));  
  YYSCopy(yyps->vs, tvs, yyps->stacksize);                  
  YYPCopy(yyps->ps, tps, yyps->stacksize);                  
  yyps->stacksize += YYSTACKGROWTH;                           
  delete[] tss;
  delete[] tvs;
  delete[] tps;
  yyps->ssp = yyps->ss + p;                                   
  yyps->vsp = yyps->vs + p;                                   
  yyps->psp = yyps->ps + p;                                   
}

yyparsestate *YYNewState(int size) {
  yyparsestate *p = new yyparsestate;
  p->stacksize = size+4;
  p->ss = new Yshort [size + 4];
  p->vs = new YYSTYPE[size + 4];
  p->ps = new YYPOSN [size + 4];
  memset(&p->vs[0], 0, (size+4)*sizeof(YYSTYPE));
  memset(&p->ps[0], 0, (size+4)*sizeof(YYPOSN));
  return p;
}

void YYFreeState(yyparsestate *p) {
  delete[] p->ss;
  delete[] p->vs;
  delete[] p->ps;
  delete p;
}
