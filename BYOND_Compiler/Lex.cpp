
#include <exception>
#include <cassert>
#include <cstdarg>
#include <cstring>
#include "Lex.hpp"

bool debug = true;
void OldLex::statistics() {

}
void OldLex::tail() {

}
void OldLex::head2() {

}
void OldLex::head1() {

}
void OldLex::follow(node_t n, node_t p) {
	if(n == tptr)
	std::visit(
		[&](auto& arg) {
			using T = std::decay_t<decltype(arg)>;
			/* will not be CHAR RNULLS FINAL S1FINAL S2FINAL RCCL RNCCL */
			if constexpr (std::is_same_v<T, RSTR>) {
				if (!n->tempstat) {
					n->tempstat = true; count++;
				}
			}
			else if constexpr (std::is_same_v<T, STAR> || std::is_same_v<T, PLUS>) {
				first(n); follow(p);
			}
			else if constexpr (std::is_same_v<T, BAR> || std::is_same_v<T, QUEST> || std::is_same_v<T, RNEWE>) {
				follow(p);
			}
			else if constexpr (std::is_same_v<T, RCAT> || std::is_same_v<T, DIV>) {
				if (v == p->left()) {
					//if (nullstr[right[p]])
				//		follow(p);
					first(p->right();
				}
			}
			else
				static_assert(always_false<T>::value, "non-exhaustive visitor!");
		}, n->value());
}
void OldLex::first(node_t n) {
	std::visit(
		[&](auto& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, CCHAR> || std::is_same_v<T, RCCL> || std::is_same_v<T, RNCCL> || std::is_same_v<T, FINAL> || std::is_same_v<T, S1FINAL> || std::is_same_v<T, S2FINAL>) {
				if (!n->tempstat) {
					n->tempstat = true; count++;
				}
			}
			else if constexpr (std::is_same_v<T, BAR> || std::is_same_v<T, RNEWE>) {
				first(arg.left); first(arg.right);
			}
			else if constexpr (std::is_same_v<T, CARAT>) {
				if (stnum % 2 == 1)
					first(arg.left);
			}
			else if constexpr (std::is_same_v<T, RSCON>) {
				int i = stnum / 2 + 1;
				for (char_t c : *arg.str) {
					if (c == i) {
						first(arg.left);
						break;
					}
				}
			}
			else if constexpr (std::is_same_v<T, STAR> || std::is_same_v<T, QUEST> || std::is_same_v<T, PLUS> || std::is_same_v<T, RSTR>) {
				first(arg.left);
			}
			else if constexpr (std::is_same_v<T, RCAT> || std::is_same_v<T, DIV>) {
				first(arg.left);
			}	
			else
				static_assert(always_false<T>::value, "non-exhaustive visitor!");
		}, n->value());
}
void OldLex::error(const char*s, ...)
{
	char buffer[1024];
	size_t pos = 0;
	va_list va;
	if (!eof) pos = snprintf(buffer, sizeof(buffer) - 1, "%d: ", yyline);
	pos += snprintf(buffer+pos, sizeof(buffer) - 1-pos, "(Error) ");
	va_start(va, s);
	pos += vsnprintf(buffer + pos, sizeof(buffer) - 1 - pos, s,va);
	va_end(va);
	pos += snprintf(buffer + pos, sizeof(buffer) - 1 - pos, "\n");
# ifdef _DEBUG
	if (debug && sect != ENDSECTION) {
		sect1dump();
		sect2dump();
	}
# endif
	if (
# ifdef _DEBUG
		debug ||
# endif
		report == 1) statistics();
	std::ostream* o = errorf ? errorf : &std::cout;
	*o << buffer;
	exit(1);	/* error return code */
}

void OldLex::warning(const char* s, ...)
{
	char buffer[1024];
	size_t pos = 0;
	va_list va;
	if (!eof) pos = snprintf(buffer, sizeof(buffer) - 1, "%d: ", yyline);
	pos += snprintf(buffer + pos, sizeof(buffer) - 1 - pos, "(Warning) ");
	va_start(va, s);
	pos += vsnprintf(buffer + pos, sizeof(buffer) - 1 - pos, s, va);
	va_end(va);
	pos += snprintf(buffer + pos, sizeof(buffer) - 1 - pos, "\n");
	std::ostream* o = errorf ? errorf : &std::cout;
	*o << buffer;
}
int OldLex::gch() {
	prev = pres;
	int c = pres = peek;
	if (unputq.empty())
		peek = fin->get();
	else {
		peek = unputq.front();
		unputq.pop();
	}
	while (peek == EOF && !filelist.empty()) {
		delete fin; // this should close?
		try {
			fin = new std::fstream(filelist.front(), std::fstream::in);
			peek = fin->get();
		}
		catch (std::exception& e) {
			if (fin) delete  fin;
			error("Exception(%s), Cannot open file %s", e.what(), filelist.front().c_str());
		}
	}
	if(c == EOF) {
		eof = true;
		if(fin) delete fin;
		return 0;
	}
	if (c == '\n') yyline++;
	return c;
}

/* returns 0 if eof is had immediately */
bool OldLex::getl(std::string& p) {
	/* return next line of input, throw away trailing '\n' */
		/* returns 0 if eof is had immediately */
	int c;
	auto len = p.size();
	while (((c = gch()) != 0) && c != '\n')
		p.push_back(c);
	if (c == 0 && len == p.size()) return false;
	prev = '\n';
	pres = '\n';
	return true;

}

void OldLex::lgate()
{
	if (lgatflg) return;
	lgatflg = true;
	if (fout == nullptr) {
		try {
			fout = new std::fstream("lex.yy.cpp", std::ios::out);
			peek = fin->get();
		}
		catch (std::exception& e) {
			if (fin) delete  fin;
			error("Exception(%s), Cannot open file lex.yy.cpp", e.what());
		}
	}
	head1();
}


void OldLex::cclinter(bool sw) {
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
}


#if 0
	/* duplicate the subtree whose root is n, return ptr to it */
	if (name < NCH) return  std::make_shared<node>(name);
	switch (name) {
	case RNULLS:
		return std::make_shared<node>(name);
	case RCCL: case RNCCL: case FINAL: case S1FINAL: case S2FINAL:
		return std::make_shared<node>(name, lstr());
	case STAR: case QUEST: case PLUS: case CARAT:
		return std::make_shared<node>(name, left()->dup());
	case RSTR: case RSCON:
		return std::make_shared<node>(name, left()->dup(), rstr());
	case BAR: case RNEWE: case RCAT: case DIV:
		return std::make_shared<node>(name, left()->dup(), right()->dup());
	}
#endif



# ifdef _DEBUG
void OldLex::allprint(int c) {
	switch (c) {
	case 014:
		printf("\\f");
		break;
	case '\n':
		printf("\\n");
		break;
	case '\t':
		printf("\\t");
		break;
	case '\b':
		printf("\\b");
		break;
	case ' ':
		printf("\\\bb");
		break;
	default:
		if (!std::isprint(c)) {
			printf("\\%-3o", c);
		}
		else
			putchar(c);
		break;
	}
}


void OldLex::sect1dump() {
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
	if (chset) {
		printf("char set changed\n");
		for (i = 1; i < NCH; i++) {
			if (i != ctable[i]) {
				allprint(i);
				putchar(' ');
				if(isprint(ctable[i]))  putchar(ctable[i]); else printf("%d", ctable[i]);
				putchar('\n');
			}
		}
	}
}
void OldLex::sect2dump() {
	printf("Sect 2:\n");
	tptr->treedump();
}
void OldLex::node::treedump(size_t ident) {
#if 0
	if (ident) 	for (size_t i = 0; i < ident; i++) putchar(' ');
#if 0
	printf("%4d ", t);
	parent[t] ? printf("p=%4d", parent[t]) : printf("      ");
	printf("  ");
#endif
	if (t->name < NCH) allprint(t->name);
	switch (t->name) {
	case RSTR:
		treedump(t->left(), ident + 1);
		allprint(t->rstr(), ident + 1);
		break;
	case RCCL:
		printf("ccl ");
		allprint(t->lstr(), ident + 1);
		break;
	case RNCCL:
		printf("nccl ");
		allprint(t->lstr(), ident + 1);
		break;
	case DIV:
		treedump(t->left(), ident + 1);
		allprint("/ \r\n", ident + 1);
		treedump(t->right(), ident + 1);
		break;
	case BAR:
		treedump(t->left(), ident + 1);
		allprint("| \r\n", ident + 1);
		treedump(t->right(), ident + 1);
		break;
	case RCAT:
		treedump(t->left(), ident + 1);
		allprint("cat \r\n", ident + 1);
		treedump(t->right(), ident + 1);
		break;
	case PLUS:
		printf("+ ");
		treedump(t->left(), ident + 1);
		break;
	case STAR:
		printf("* ");
		treedump(t->left(), ident + 1);
		break;
	case CARAT:
		printf("^ ");
		treedump(t->left(), ident + 1);
		break;
	case QUEST:
		printf("? ");
		treedump(t->left(), ident + 1);
		break;
	case RNULLS:
		printf("nullstring");
		break;
	case FINAL:
		printf("final ");
		treedump(t->left(), ident + 1);
		break;
	case S1FINAL:
		printf("s1final ");
		treedump(t->left(), ident + 1);
		break;
	case S2FINAL:
		printf("s2final ");
		treedump(t->left(), ident + 1);
		break;
	case RNEWE:
		printf("new ");
		treedump(t->left(), ident + 1);
		break;
	case RSCON:
		printf("start ");
		allprint(t->rstr(), ident + 1);
#if 0
		p = right[t];
		printf("start %s", sname[*p++ - 1]);
		while (*p)
			printf(", %s", sname[*p++ - 1]);
#endif
		allprint("rscon? \r\n", ident + 1);
		treedump(t->left(), ident + 1);
		break;
	default:
		assert(0);
#if 0
		printf("unknown %d %d %d", name[t], left[t], right[t]);
#endif
		break;
	}
	if (t->nullstr) printf("\t(null poss.)");
	putchar('\n');
#endif
}

# endif

