
#include "defs.h"

extern Yshort *itemset;
extern Yshort *itemsetend;
extern unsigned *ruleset;

int nstates;
core *first_state;
shifts *first_shift;
reductions *first_reduction;

static core **state_set;
static core *this_state;
static core *last_state;
static shifts *last_shift;
static reductions *last_reduction;

static int nshifts;
static Yshort *shift_symbol;

static Yshort *redset;
static Yshort *shiftset;

static Yshort **kernel_base;
static Yshort **kernel_end;
static Yshort *kernel_items;

template<typename T, typename AT>
static inline T* allocate(size_t array_count) {
	AT* tmp = new AT[sizeof(T) + ((array_count - 1) * sizeof(AT))];
	return new(tmp) T;
}

void allocate_itemsets()
{
     Yshort *itemp;
     Yshort *item_end;
     int symbol;
     int i;
     int count;
     int max;
     Yshort *symbol_count;

    count = 0;
    symbol_count = NEW2(nsyms, Yshort);

    item_end = ritem + nitems;
    for (itemp = ritem; itemp < item_end; itemp++)
    {
	symbol = *itemp;
	if (symbol >= 0)
	{
	    count++;
	    symbol_count[symbol]++;
	}
    }

    kernel_base = NEW2(nsyms, Yshort *);
    kernel_items = NEW2(count, Yshort);

    count = 0;
    max = 0;
    for (i = 0; i < nsyms; i++)
    {
	kernel_base[i] = kernel_items + count;
	count += symbol_count[i];
	if (max < symbol_count[i])
	    max = symbol_count[i];
    }

    shift_symbol = symbol_count;
    kernel_end = NEW2(nsyms, Yshort *);
}


void allocate_storage()
{
    allocate_itemsets();
    shiftset = NEW2(nsyms, Yshort);
    redset = NEW2(nrules + 1, Yshort);
    state_set = NEW2(nitems, core *);
}


void append_states()
{
     int i;
     int j;
     int symbol;

#ifdef	TRACE
    fprintf(stderr, "Entering append_states()\n");
#endif
    for (i = 1; i < nshifts; i++)
    {
	symbol = shift_symbol[i];
	j = i;
	while (j > 0 && shift_symbol[j - 1] > symbol)
	{
	    shift_symbol[j] = shift_symbol[j - 1];
	    j--;
	}
	shift_symbol[j] = symbol;
    }

    for (i = 0; i < nshifts; i++)
    {
	symbol = shift_symbol[i];
	shiftset[i] = get_state(symbol);
    }
}


void free_storage()
{
    FREE(shift_symbol);
    FREE(redset);
    FREE(shiftset);
    FREE(kernel_base);
    FREE(kernel_end);
    FREE(kernel_items);
    FREE(state_set);
}



void generate_states()
{
    allocate_storage();
    itemset = NEW2(nitems, Yshort);
    ruleset = NEW2(WORDSIZE(nrules), unsigned);
    set_first_derives();
    initialize_states();

    while (this_state)
    {
	closure(this_state->items, this_state->nitems);
	save_reductions();
	new_itemsets();
	append_states();

	if (nshifts > 0)
	    save_shifts();

	this_state = this_state->next;
    }

    finalize_closure();
    free_storage();
}



int get_state(int symbol)
{
     int key;
     Yshort *isp1;
     Yshort *isp2;
     Yshort *iend;
     core *sp;
     int found;
     int n;

#ifdef	TRACE
    fprintf(stderr, "Entering get_state(%d)\n", symbol);
#endif

    isp1 = kernel_base[symbol];
    iend = kernel_end[symbol];
    n = iend - isp1;

    key = *isp1;
    assert(0 <= key && key < nitems);
    sp = state_set[key];
    if (sp)
    {
	found = 0;
	while (!found)
	{
	    if (sp->nitems == n)
	    {
		found = 1;
		isp1 = kernel_base[symbol];
		isp2 = sp->items;

		while (found && isp1 < iend)
		{
		    if (*isp1++ != *isp2++)
			found = 0;
		}
	    }

	    if (!found)
	    {
		if (sp->link)
		{
		    sp = sp->link;
		}
		else
		{
		    sp = sp->link = new_state(symbol);
		    found = 1;
		}
	    }
	}
    }
    else
    {
	state_set[key] = sp = new_state(symbol);
    }

    return (sp->number);
}



void initialize_states()
{
     int i;
     Yshort *start_derives;
     core *p;

    start_derives = derives[start_symbol];
    for (i = 0; start_derives[i] >= 0; ++i)
	continue;

    p = (core *) MALLOC(sizeof(core) + i*sizeof(Yshort));
    if (p == 0) no_space();

    p->next = 0;
    p->link = 0;
    p->number = 0;
    p->accessing_symbol = 0;
    p->nitems = i;

    for (i = 0;  start_derives[i] >= 0; ++i)
	p->items[i] = rrhs[start_derives[i]];

    first_state = last_state = this_state = p;
    nstates = 1;
}


void new_itemsets()
{
     int i;
     int shiftcount;
     Yshort *isp;
     Yshort *ksp;
     int symbol;

    for (i = 0; i < nsyms; i++)
	kernel_end[i] = 0;

    shiftcount = 0;
    isp = itemset;
    while (isp < itemsetend)
    {
	i = *isp++;
	symbol = ritem[i];
	if (symbol > 0)
	{
	    ksp = kernel_end[symbol];
	    if (!ksp)
	    {
		shift_symbol[shiftcount++] = symbol;
		ksp = kernel_base[symbol];
	    }

	    *ksp++ = i + 1;
	    kernel_end[symbol] = ksp;
	}
    }

    nshifts = shiftcount;
}




core *new_state(int symbol)
{
     int n;
     core *p;
     Yshort *isp1;
     Yshort *isp2;
     Yshort *iend;

#ifdef	TRACE
    fprintf(stderr, "Entering new_state(%d)\n", symbol);
#endif

    if (nstates >= MAXSHORT)
	fatal("too many states");

    isp1 = kernel_base[symbol];
    iend = kernel_end[symbol];
    n = iend - isp1;

	p = allocate< core, Yshort>(n);
    p->accessing_symbol = symbol;
    p->number = nstates;
    p->nitems = n;

    isp2 = p->items;
    while (isp1 < iend)
	*isp2++ = *isp1++;

    last_state->next = p;
    last_state = p;

    nstates++;

    return (p);
}


/* show_cores is used for debugging */

void show_cores()
{
    core *p;
    int i, j, k, n;
    int itemno;

    k = 0;
    for (p = first_state; p; ++k, p = p->next)
    {
	if (k) printf("\n");
	printf("state %d, number = %d, accessing symbol = %s\n",
		k, p->number, symbol_name[p->accessing_symbol]);
	n = p->nitems;
	for (i = 0; i < n; ++i)
	{
	    itemno = p->items[i];
	    printf("%4d  ", itemno);
	    j = itemno;
	    while (ritem[j] >= 0) ++j;
	    printf("%s :", symbol_name[rlhs[-ritem[j]]]);
	    j = rrhs[-ritem[j]];
	    while (j < itemno)
		printf(" %s", symbol_name[ritem[j++]]);
	    printf(" .");
	    while (ritem[j] >= 0)
		printf(" %s", symbol_name[ritem[j++]]);
	    printf("\n");
	    fflush(stdout);
	}
    }
}


/* show_ritems is used for debugging */

void show_ritems()
{
    int i;

    for (i = 0; i < nitems; ++i)
	printf("ritem[%d] = %d\n", i, ritem[i]);
}


/* show_rrhs is used for debugging */
void show_rrhs()
{
    int i;

    for (i = 0; i < nrules; ++i)
	printf("rrhs[%d] = %d\n", i, rrhs[i]);
}


/* show_shifts is used for debugging */

void show_shifts()
{
    shifts *p;
    int i, j, k;

    k = 0;
    for (p = first_shift; p; ++k, p = p->next)
    {
	if (k) printf("\n");
	printf("shift %d, number = %d, nshifts = %d\n", k, p->number,
		p->nshifts);
	j = p->nshifts;
	for (i = 0; i < j; ++i)
	    printf("\t%d\n", p->shift[i]);
    }
}


void save_shifts()
{
     shifts *p;
     Yshort *sp1;
     Yshort *sp2;
     Yshort *send;


	 p = allocate<shifts, Yshort>(nshifts);


    p->number = this_state->number;
    p->nshifts = nshifts;

    sp1 = shiftset;
    sp2 = p->shift;
    send = shiftset + nshifts;

    while (sp1 < send)
	*sp2++ = *sp1++;

    if (last_shift)
    {
	last_shift->next = p;
	last_shift = p;
    }
    else
    {
	first_shift = p;
	last_shift = p;
    }
}



void save_reductions()
{
	Yshort* isp;
	Yshort* rp1;
	Yshort* rp2;
	int item;
	int count;
	reductions* p;
	Yshort* rend;

	count = 0;
	for (isp = itemset; isp < itemsetend; isp++)
	{
		item = ritem[*isp];
		if (item < 0)
		{
			redset[count++] = -item;
		}
	}

	if (count)
	{
		p = allocate<reductions, Yshort>(count);


		p->number = this_state->number;
		p->nreds = count;

		rp1 = redset;
		rp2 = p->rules;
		rend = rp1 + count;

		while (rp1 < rend)
			* rp2++ = *rp1++;

		if (last_reduction)
		{
			last_reduction->next = p;
			last_reduction = p;
		}
		else
		{
			first_reduction = p;
			last_reduction = p;
		}
	}
}


void set_derives()
{
     int i, k;
     int lhs;
     Yshort *rules;

    derives = NEW2(nsyms, Yshort *);
    rules = NEW2(nvars + nrules, Yshort);

    k = 0;
    for (lhs = start_symbol; lhs < nsyms; lhs++)
    {
	derives[lhs] = rules + k;
	for (i = 0; i < nrules; i++)
	{
	    if (rlhs[i] == lhs)
	    {
		rules[k] = i;
		k++;
	    }
	}
	rules[k] = -1;
	k++;
    }

#ifdef	DEBUG
    print_derives();
#endif
}

void free_derives()
{
    FREE(derives[start_symbol]);
    FREE(derives);
}

#ifdef	DEBUG
void print_derives()
{
     int i;
     Yshort *sp;

    printf("\nDERIVES\n\n");

    for (i = start_symbol; i < nsyms; i++)
    {
	printf("%s derives ", symbol_name[i]);
	for (sp = derives[i]; *sp >= 0; sp++)
	{
	    printf("  %d", *sp);
	}
	putchar('\n');
    }

    putchar('\n');
}
#endif


void set_nullable()
{
     int i, j;
     int empty;
    int done;

	nullable.resize(nsyms);
	
    for (int i = 0; i < nsyms; ++i)
		nullable[i] = false;

    done = 0;
    while (!done)
    {
	done = 1;
	for (i = 1; i < nitems; i++)
	{
	    empty = 1;
	    while ((j = ritem[i]) >= 0)
	    {
		if (!nullable[j])
		    empty = 0;
		++i;
	    }
	    if (empty)
	    {
		j = rlhs[-j];
		if (!nullable[j])
		{
		    nullable[j] = true;
		    done = 0;
		}
	    }
	}
    }

#ifdef DEBUG
    for (i = 0; i < nsyms; i++)
    {
	if (nullable[i])
	    printf("%s is nullable\n", symbol_name[i]);
	else
	    printf("%s is not nullable\n", symbol_name[i]);
    }
#endif
}


void free_nullable()
{
	nullable.clear();
}


void lr0()
{
    set_derives();
    set_nullable();
    generate_states();
}
