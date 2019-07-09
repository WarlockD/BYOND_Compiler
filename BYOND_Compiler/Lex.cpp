#include "Lex.hpp"
#include <exception>
#include <cassert>
#include <cstdarg>
#include <cstring>


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
	return;
}
OldLex::node::node(int n, nodep l, nodep r) : name(n), _left(l), _right(r), parent(nullptr), nullstr(false) {
	switch (n) {
	case BAR:
	case RNEWE:
		if (l->nullstr || r->nullstr) nullstr = true;
		l->parent = r->parent = this->shared_from_this();
		break;
	case RCAT:
	case DIV:
		if (l->nullstr && r->nullstr) nullstr = true;
		l->parent = r->parent = this->shared_from_this();
		break;
	default:
#ifdef _DEBUG
		throw std::string("bad switch mn2 ") + std::to_string(n);
		//warning("bad switch mn2 %d %d", a, d);
		break;
#endif	
	};
}

OldLex::node::node(int n, nodep l, std::string_view  r) {
	switch (n) {
	case RSTR:
		l->parent = this->shared_from_this();
		break;
	case RSCON:
		l->parent = this->shared_from_this();
		nullstr = l->nullstr;
		break;
	default:
#ifdef _DEBUG
		throw std::string("bad switch mn2 ") + std::to_string(n);
		//warning("bad switch mn2 %d %d", a, d);
		break;
#endif	
	};
}
OldLex::node::node(int n, nodep l) : name(n), _left(l), _right(nullptr), parent(nullptr), nullstr(false) {
	switch (n) {
	case STAR:
	case QUEST:
		nullstr = true;
		l->parent = this->shared_from_this();
		break;
	case PLUS:
	case CARAT:
		nullstr = l->nullstr;
		l->parent = this->shared_from_this();
		break;
	case S2FINAL:
		nullstr = true;
		break;
# ifdef _DEBUG
	case FINAL:
	case S1FINAL:
		break;
	default:
		throw std::string("bad switch mn1 ") + std::to_string(n);
		//warning("bad switch mn1 %d %d", a, d);
		break;
# endif
	}
}
OldLex::node::node(int n, std::string_view l) : name(n), _left(l), _right(nullptr), parent(nullptr), nullstr(false) {
	switch (n) {
	case RCCL:
	case RNCCL:
		if (l.size() == 0) nullstr = true;
		break;
# ifdef _DEBUG
	case FINAL:
	case S1FINAL:
		break;
	default:
		throw std::string("bad switch mn1 ") + std::to_string(n);
		//warning("bad switch mn1 %d %d", a, d);
		break;
# endif
	}
}
OldLex::node::node(int n) : name(n), _left(std::shared_ptr<node>(nullptr)), _right(nullptr), parent(nullptr), nullstr(RNULLS == n) {
# ifdef _DEBUG
	if (n >= NCH) {
		throw std::string("bad switch mn0 ") + std::to_string(n);
		//warning("bad switch mn1 %d %d", a, d);
	}
# endif
}
OldLex::nodep OldLex::node::dup() const {
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
}


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
	return;
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
	treedump();
}
void OldLex::treedump(node::nodep t, size_t ident) {
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
}
void OldLex::treedump() {
	node::nodep t = tptr;
	treedump(t, 0);
}
# endif
