#include "defs.h"

typedef
  struct shorts
    {
      struct shorts *next;
      Yshort value;
    }
  shorts;

int tokensetsize;
Yshort *lookaheads;
Yshort *LAruleno;
unsigned *LA;
Yshort *accessing_symbol;
core **state_table;
shifts **shift_table;
reductions **reduction_table;
Yshort *goto_map;
Yshort *from_state;
Yshort *to_state;

static int infinity;
static int maxrhs;
static int ngotos;
static unsigned *F;
static Yshort **includes;
static shorts **lookback;
static Yshort **R;
static Yshort *INDEX;
static Yshort *VERTICES;
static int top;


void lalr()
{
    tokensetsize = WORDSIZE(ntokens);

    set_state_table();
    set_accessing_symbol();
    set_shift_table();
    set_reduction_table();
    set_maxrhs();
    initialize_LA();
    set_goto_map();
    initialize_F();
    build_relations();
    compute_FOLLOWS();
    compute_lookaheads();
}



void set_state_table()
{
     core *sp;

    state_table = NEW2(nstates, core *);
    for (sp = first_state; sp; sp = sp->next)
	state_table[sp->number] = sp;
}



void set_accessing_symbol()
{
     core *sp;

    accessing_symbol = NEW2(nstates, Yshort);
    for (sp = first_state; sp; sp = sp->next)
	accessing_symbol[sp->number] = sp->accessing_symbol;
}



void set_shift_table()
{
     shifts *sp;

    shift_table = NEW2(nstates, shifts *);
    for (sp = first_shift; sp; sp = sp->next)
	shift_table[sp->number] = sp;
}



void set_reduction_table()
{
     reductions *rp;

    reduction_table = NEW2(nstates, reductions *);
    for (rp = first_reduction; rp; rp = rp->next)
	reduction_table[rp->number] = rp;
}



void set_maxrhs()
{
   Yshort *itemp;
   Yshort *item_end;
   int length;
   int max;

  length = 0;
  max = 0;
  item_end = ritem + nitems;
  for (itemp = ritem; itemp < item_end; itemp++)
    {
      if (*itemp >= 0)
	{
	  length++;
	}
      else
	{
	  if (length > max) max = length;
	  length = 0;
	}
    }

  maxrhs = max;
}



void initialize_LA()
{
   int i, j, k;
   reductions *rp;

  lookaheads = NEW2(nstates + 1, Yshort);

  k = 0;
  for (i = 0; i < nstates; i++)
    {
      lookaheads[i] = k;
      rp = reduction_table[i];
      if (rp)
	k += rp->nreds;
    }
  lookaheads[nstates] = k;

  LA = NEW2(k * tokensetsize, unsigned);
  LAruleno = NEW2(k, Yshort);
  lookback = NEW2(k, shorts *);

  k = 0;
  for (i = 0; i < nstates; i++)
    {
      rp = reduction_table[i];
      if (rp)
	{
	  for (j = 0; j < rp->nreds; j++)
	    {
	      LAruleno[k] = rp->rules[j];
	      k++;
	    }
	}
    }
}


void set_goto_map()
{
   shifts *sp;
   int i;
   int symbol;
   int k;
   Yshort *temp_map;
   int state2;
   int state1;

  goto_map = NEW2(nvars + 1, Yshort) - ntokens;
  temp_map = NEW2(nvars + 1, Yshort) - ntokens;

  ngotos = 0;
  for (sp = first_shift; sp; sp = sp->next)
    {
      for (i = sp->nshifts - 1; i >= 0; i--)
	{
	  symbol = accessing_symbol[sp->shift[i]];

	  if (ISTOKEN(symbol)) break;

	  if (ngotos == MAXSHORT)
	    fatal("too many gotos");

	  ngotos++;
	  goto_map[symbol]++;
        }
    }

  k = 0;
  for (i = ntokens; i < nsyms; i++)
    {
      temp_map[i] = k;
      k += goto_map[i];
    }

  for (i = ntokens; i < nsyms; i++)
    goto_map[i] = temp_map[i];

  goto_map[nsyms] = ngotos;
  temp_map[nsyms] = ngotos;

  from_state = NEW2(ngotos, Yshort);
  to_state = NEW2(ngotos, Yshort);

  for (sp = first_shift; sp; sp = sp->next)
    {
      state1 = sp->number;
      for (i = sp->nshifts - 1; i >= 0; i--)
	{
	  state2 = sp->shift[i];
	  symbol = accessing_symbol[state2];

	  if (ISTOKEN(symbol)) break;

	  k = temp_map[symbol]++;
	  from_state[k] = state1;
	  to_state[k] = state2;
	}
    }

  FREE(temp_map + ntokens);
}



/*  Map_goto maps a state/symbol pair into its numeric representation.	*/

int map_goto(int state, int symbol)
{
     int high;
     int low;
     int middle;
     int s;

    low = goto_map[symbol];
    high = goto_map[symbol + 1];

    for (;;)
    {
	assert(low <= high);
	middle = (low + high) >> 1;
	s = from_state[middle];
	if (s == state)
	    return (middle);
	else if (s < state)
	    low = middle + 1;
	else
	    high = middle - 1;
    }
}



void initialize_F()
{
   int i;
   int j;
   int k;
   shifts *sp;
   Yshort *edge;
   unsigned *rowp;
   Yshort *rp;
   Yshort **reads;
   int nedges;
   int stateno;
   int symbol;
   int nwords;

  nwords = ngotos * tokensetsize;
  F = NEW2(nwords, unsigned);

  reads = NEW2(ngotos, Yshort *);
  edge = NEW2(ngotos + 1, Yshort);
  nedges = 0;

  rowp = F;
  for (i = 0; i < ngotos; i++)
    {
      stateno = to_state[i];
      sp = shift_table[stateno];

      if (sp)
	{
	  k = sp->nshifts;

	  for (j = 0; j < k; j++)
	    {
	      symbol = accessing_symbol[sp->shift[j]];
	      if (ISVAR(symbol))
		break;
	      SETBIT(rowp, symbol);
	    }

	  for (; j < k; j++)
	    {
	      symbol = accessing_symbol[sp->shift[j]];
	      if (nullable[symbol])
		edge[nedges++] = map_goto(stateno, symbol);
	    }
	
	  if (nedges)
	    {
	      reads[i] = rp = NEW2(nedges + 1, Yshort);

	      for (j = 0; j < nedges; j++)
		rp[j] = edge[j];

	      rp[nedges] = -1;
	      nedges = 0;
	    }
	}

      rowp += tokensetsize;
    }

  SETBIT(F, 0);
  digraph(reads);

  for (i = 0; i < ngotos; i++)
    {
      if (reads[i])
	FREE(reads[i]);
    }

  FREE(reads);
  FREE(edge);
}



void build_relations()
{
   int i;
   int j;
   int k;
   Yshort *rulep;
   Yshort *rp;
   shifts *sp;
   int length;
   int nedges;
   int done;
   int state1;
   int stateno;
   int symbol1;
   int symbol2;
   Yshort *shortp;
   Yshort *edge;
   Yshort *states;
   Yshort **new_includes;

  includes = NEW2(ngotos, Yshort *);
  edge = NEW2(ngotos + 1, Yshort);
  states = NEW2(maxrhs + 1, Yshort);

  for (i = 0; i < ngotos; i++)
    {
      nedges = 0;
      state1 = from_state[i];
      symbol1 = accessing_symbol[to_state[i]];

      for (rulep = derives[symbol1]; *rulep >= 0; rulep++)
	{
	  length = 1;
	  states[0] = state1;
	  stateno = state1;

	  for (rp = ritem + rrhs[*rulep]; *rp >= 0; rp++)
	    {
	      symbol2 = *rp;
	      sp = shift_table[stateno];
	      k = sp->nshifts;

	      for (j = 0; j < k; j++)
		{
		  stateno = sp->shift[j];
		  if (accessing_symbol[stateno] == symbol2) break;
		}

	      states[length++] = stateno;
	    }

	  add_lookback_edge(stateno, *rulep, i);

	  length--;
	  done = 0;
	  while (!done)
	    {
	      done = 1;
	      rp--;
	      if (ISVAR(*rp))
		{
		  stateno = states[--length];
		  edge[nedges++] = map_goto(stateno, *rp);
		  if (nullable[*rp] && length > 0) done = 0;
		}
	    }
	}

      if (nedges)
	{
	  includes[i] = shortp = NEW2(nedges + 1, Yshort);
	  for (j = 0; j < nedges; j++)
	    shortp[j] = edge[j];
	  shortp[nedges] = -1;
	}
    }

  new_includes = transpose(includes, ngotos);

  for (i = 0; i < ngotos; i++)
    if (includes[i])
      FREE(includes[i]);

  FREE(includes);

  includes = new_includes;

  FREE(edge);
  FREE(states);
}


void add_lookback_edge(int stateno, int ruleno, int gotono)
{
     int i, k;
     int found;
     shorts *sp;

    i = lookaheads[stateno];
    k = lookaheads[stateno + 1];
    found = 0;
    while (!found && i < k)
    {
	if (LAruleno[i] == ruleno)
	    found = 1;
	else
	    ++i;
    }
    assert(found);

    sp = NEW(shorts);
    sp->next = lookback[i];
    sp->value = gotono;
    lookback[i] = sp;
}



Yshort **transpose(Yshort **R, int n)
{
   Yshort **new_R;
   Yshort **temp_R;
   Yshort *nedges;
   Yshort *sp;
   int i;
   int k;

  nedges = NEW2(n, Yshort);

  for (i = 0; i < n; i++)
    {
      sp = R[i];
      if (sp)
	{
	  while (*sp >= 0)
	    nedges[*sp++]++;
	}
    }

  new_R = NEW2(n, Yshort *);
  temp_R = NEW2(n, Yshort *);

  for (i = 0; i < n; i++)
    {
      k = nedges[i];
      if (k > 0)
	{
	  sp = NEW2(k + 1, Yshort);
	  new_R[i] = sp;
	  temp_R[i] = sp;
	  sp[k] = -1;
	}
    }

  FREE(nedges);

  for (i = 0; i < n; i++)
    {
      sp = R[i];
      if (sp)
	{
	  while (*sp >= 0)
	    *temp_R[*sp++]++ = i;
	}
    }

  FREE(temp_R);

  return (new_R);
}



void compute_FOLLOWS()
{
  digraph(includes);
}


void compute_lookaheads()
{
   int i, n;
   unsigned *fp1, *fp2, *fp3;
   shorts *sp, *next;
   unsigned *rowp;

  rowp = LA;
  n = lookaheads[nstates];
  for (i = 0; i < n; i++)
    {
      fp3 = rowp + tokensetsize;
      for (sp = lookback[i]; sp; sp = sp->next)
	{
	  fp1 = rowp;
	  fp2 = F + tokensetsize * sp->value;
	  while (fp1 < fp3)
	    *fp1++ |= *fp2++;
	}
      rowp = fp3;
    }

  for (i = 0; i < n; i++)
    for (sp = lookback[i]; sp; sp = next)
      {
        next = sp->next;
        FREE(sp);
      }

  FREE(lookback);
  FREE(F);
}


void digraph(Yshort **relation)
{
   int i;

  infinity = ngotos + 2;
  INDEX = NEW2(ngotos + 1, Yshort);
  VERTICES = NEW2(ngotos + 1, Yshort);
  top = 0;

  R = relation;

  for (i = 0; i < ngotos; i++)
    INDEX[i] = 0;

  for (i = 0; i < ngotos; i++)
    {
      if (INDEX[i] == 0 && R[i])
	traverse(i);
    }

  FREE(INDEX);
  FREE(VERTICES);
}

void traverse(int i)
{
   unsigned *fp1;
   unsigned *fp2;
   unsigned *fp3;
   int j;
   Yshort *rp;

  int height;
  unsigned *base;

  VERTICES[++top] = i;
  INDEX[i] = height = top;

  base = F + i * tokensetsize;
  fp3 = base + tokensetsize;

  rp = R[i];
  if (rp)
    {
      while ((j = *rp++) >= 0)
	{
	  if (INDEX[j] == 0)
	    traverse(j);

	  if (INDEX[i] > INDEX[j])
	    INDEX[i] = INDEX[j];

	  fp1 = base;
	  fp2 = F + j * tokensetsize;

	  while (fp1 < fp3)
	    *fp1++ |= *fp2++;
	}
    }

  if (INDEX[i] == height)
    {
      for (;;)
	{
	  j = VERTICES[top--];
	  INDEX[j] = infinity;

	  if (i == j)
	    break;

	  fp1 = base;
	  fp2 = F + j * tokensetsize;

	  while (fp1 < fp3)
	    *fp2++ = *fp1++;
	}
    }
}
