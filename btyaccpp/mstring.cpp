#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "mstring.h"


static std::string_view string_t::_lookup(std::string_view sv) {
	if (sv.size() == 0) return empty_string;
	static std::unordered_map<std::string_view, std::unique_ptr<const char>> string_table;
	auto it = string_table.find(sv);
	if (it != string_table.end()) return *(it->second);
	char* str = new char[sv.size() + 1];
	sv.copy(str, sv.size());
	str[sv.size()] = '\0';
	std::string_view ret = std::string_view(str, sv.size());
	string_table.emplace(ret, str);
	return ret;
}

#if 0
/* parameters about string length.  START is the starting size and
** START+TAIL should be a power of two */
#define START	24
#define TAIL	8

void msprintf(struct mstring *s, const char *fmt, ...)
{
static char	buf[4096];	/* a big static buffer */
va_list		args;
int		len;

    if (!s || !s->base) return;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    len = strlen(buf);
    if (len > (s->end - s->ptr)) {
	int	cp = s->ptr - s->base, cl = s->end - s->base, nl = cl;
	while (len > (nl - cp))
	    nl = nl + nl + TAIL;
	if ((s->base = (char*)realloc(s->base, nl))) {
	    s->ptr = s->base + cp;
	    s->end = s->base + nl; }
	else {
	    s->ptr = s->end = 0;
	    return; } }
    memcpy(s->ptr, buf, len);
    s->ptr += len;
}

int mputchar(struct mstring *s, int ch)
{
    if (!s || !s->base) return ch;
    if (s->ptr == s->end) {
	int len = s->end - s->base;
	if ((s->base = (char*)realloc(s->base, len+len+TAIL))) {
	    s->ptr = s->base + len;
	    s->end = s->base + len+len+TAIL; }
	else {
	    s->ptr = s->end = 0;
	    return ch; } }
    *s->ptr++ = ch;
    return ch;
}

struct mstring *msnew(void) {
    struct mstring *n = (char*)malloc(sizeof(struct mstring));

    if (n && (n->base = n->ptr = (char*)malloc(START)))
	n->end = n->base + START;
    else if (n) {
	free(n);
	n = 0; }
    return n;
}

char *msdone(struct mstring *s)
{
char	*r = 0;
    if (s) {
	mputc(s, 0);
	r = s->base;
	free(s); }
    return r;
}

#endif

/* compare two strings, ignoring whitespace, except between two letters or
** digits (and treat all of these as equal) */
int strnscmp(const char *a, const char *b)
{
    while(1) {
	while (isspace(*a)) a++;
	while (isspace(*b)) b++;
	while (*a && *a == *b) a++,b++;
	if (isspace(*a)) {
	    if (isalnum(a[-1]) && isalnum(*b))
		break; }
	else if (isspace(*b)) {
	    if (isalnum(b[-1]) && isalnum(*a))
		break; }
	else
	    break; }
    return *a - *b;
}

unsigned int strnshash(const char *s)
{
unsigned int	h = 0;

    while (*s) {
	if (!isspace(*s))
	    h = (h<<5) - h + *s;
	s++; }
    return h;
}
