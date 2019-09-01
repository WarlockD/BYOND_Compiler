#include "defs.h"


/* TABLE_SIZE is the number of entries in the symbol table. */
/* TABLE_SIZE must be a power of two.			    */

#define	TABLE_SIZE 1024


//bucket **symbol_table;
//bucket *first_symbol;
//bucket *last_symbol;


std::forward_list<bucket> symbol_list;
bucket last_inserted;
std::unordered_map<string_t, bucket> symbol_table;



static bucket make_bucket(string_t name) {
	last_inserted = symbol_list.emplace_after(last_inserted, name);
	symbol_table.emplace(name,last_inserted);
    return last_inserted;
}



bucket lookup(string_t name)
{
	if (auto it = symbol_table.find(name); it != symbol_table.end())
		return it->second;
	else
		make_bucket(name);
}


void create_symbol_table()
{
    auto bp = make_bucket("error");
    bp->index = 1;
    bp->cclass = TERM;
}


