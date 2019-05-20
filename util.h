#ifndef __UTIL_HEADER_INCLUDED__
#define __UTIL_HEADER_INCLUDED__
#include "sds/sds.h"

void *xmalloc(size_t);
void xfree(void **);
double parse_double(sds);

#define xnew(T) (xmalloc(sizeof(T)))

#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

#define GenParseNumberProtWithName(T, Name) T parse_##Name(sds str);
#define GenParseNumberProt(T) GenParseNumberProtWithName(T, T)

GenParseNumberProt(int);
GenParseNumberProt(size_t);

#include "vector.h"

sds vecstrjoin(Vector *strs, sds sep);

#endif
