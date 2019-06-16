#ifndef __UTIL_HEADER_INCLUDED__
#define __UTIL_HEADER_INCLUDED__
#include "sds/sds.h"
#include <limits.h>

typedef void (*ELEM_DESTRUCTOR)(void *);
typedef int (*ELEM_COMPARE)(void *, void *);
typedef char *(*ELEM_PRINTER)(void *);

void *xmalloc(size_t);
#define xfree(ptr_p) (xfreeImpl((void **)ptr_p))
void xfreeImpl(void **);
double parseDouble(sds);

#define xnew(T) (xmalloc(sizeof(T)))

#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

#define GenParseNumberProtWithName(T, Name) T parse_##Name(sds str);
#define GenParseNumberProt(T) GenParseNumberProtWithName(T, T)

GenParseNumberProt(int);
GenParseNumberProt(size_t);

#include "vector.h"

sds vecstrjoin(Vector *strs, sds sep);

size_t checked_size_sub(size_t a, size_t b);

size_t checked_size_add(size_t a, size_t b);

typedef struct {
  void *data;
  size_t size;
} SizedData;

sds readText(const sds file_name);

#define GenNumericToLowers(T, R, T_NAME, R_NAME, R_MAX)                        \
  static inline R *T_NAME##_to_##R_NAME##s(T v) {                              \
    size_t T_size = sizeof(T);                                                 \
    size_t R_size = sizeof(R);                                                 \
    size_t rets_length = T_size / R_size;                                      \
    R *rets = xmalloc(sizeof(R) * rets_length);                                \
                                                                               \
    for (size_t i = 0; i < rets_length; i++) {                                 \
      R t = (v >> (R_size * 8 * i)) & R_MAX;                                   \
      rets[i] = t;                                                             \
    }                                                                          \
                                                                               \
    return rets;                                                               \
  }

#define GenLowersToNumeric(T, R, T_NAME, R_NAME)                               \
  static inline R T_NAME##s_to_##R_NAME(T *inputs) {                           \
    R v = 0;                                                                   \
    size_t T_size = sizeof(T);                                                 \
    size_t R_size = sizeof(R);                                                 \
    size_t iter_end = R_size / T_size;                                         \
                                                                               \
    for (size_t i = 0; i < iter_end; i++) {                                    \
      v |= ((R)inputs[i]) << (T_size * 8 * i);                                 \
    }                                                                          \
                                                                               \
    return v;                                                                  \
  }

// int <-> unsigned char
GenNumericToLowers(int, unsigned char, int, uchar, UCHAR_MAX);
GenLowersToNumeric(unsigned char, int, uchar, int);

// unsigned iong long int <-> unsigned char
GenNumericToLowers(unsigned long long int, unsigned char, ulli, uchar,
                   UCHAR_MAX);
GenLowersToNumeric(unsigned char, unsigned long long int, uchar, ulli);
#endif
