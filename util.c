#include "cplayground.h"
#include "sds/sds.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Failed to allocate memory\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void xfree(void **p_ptr) {
  if (p_ptr == NULL || *p_ptr == NULL) {
    fprintf(stderr, "Given pointer is NULL");
    exit(EXIT_FAILURE);
  }

  free(*p_ptr);
  *p_ptr = NULL;
}

static double dpow(double n, size_t p) {
  double t = 1.;
  for (size_t i = 0; i < p; i++) {
    t *= n;
  }
  return t;
}

#define ASCII_NUM_TO_INT(i) (i - '0')

double parse_double(sds str) {
  Vector *d = new_vec();
  Vector *f = new_vec();
  double ret = 0;
  double neg = false;
  size_t cursor = 0;

  if (strlen(str) == 0) {
    return ret; // ret == 0
  }

  if (str[0] == '-') {
    neg = true;
    cursor++;
  }

  for (; cursor < strlen(str) && str[cursor] != '.'; cursor++) {
    vec_push(d, INT_TO_VoPTR(str[cursor]));
  }

  size_t d_len = d->len;
  for (size_t i = 0; i < d_len; i++) {
    size_t j = d_len - i - 1;
    double v = ASCII_NUM_TO_INT(VoPTR_TO_INT(d->data[i])) * dpow(10, j);
    ret += v;
  }

  if (cursor != strlen(str) - 1) {
    cursor += 1; // skip dot
    for (; cursor < strlen(str); cursor++) {
      vec_push(f, (void *)(intptr_t)str[cursor]);
    }

    size_t f_len = f->len;
    for (size_t i = 0; i < f_len; i++) {
      size_t j = i + 1;
      double v = ASCII_NUM_TO_INT(VoPTR_TO_INT(f->data[i])) / dpow(10, j);
      ret += v;
    }
  }

  if (neg) {
    ret *= -1;
  }

  return ret;
}

#define GenIPowWithName(T, Name)                                               \
  static T ipow_##Name(T n, T p) {                                             \
    T t = 1;                                                                   \
    for (T i = 0; i < p; i++) {                                                \
      t *= n;                                                                  \
    }                                                                          \
    return t;                                                                  \
  }

#define GenIPow(T) GenIPowWithName(T, T)

#define GenParseNumberWithName(T, Name)                                        \
  GenIPowWithName(T, Name);                                                    \
  T parse_##Name(sds str) {                                                    \
    T ret = 0;                                                                 \
    size_t len = strlen(str);                                                  \
                                                                               \
    for (size_t i = 0; i < len; i++) {                                         \
      if (!isdigit(str[i])) {                                                  \
        fprintf(stderr, "Failed to parse %s\n", #T);                           \
        exit(EXIT_FAILURE);                                                    \
      }                                                                        \
      T t = str[i] - '0';                                                      \
      T j = len - i - 1;                                                       \
      ret += t * ipow_##Name(10, j);                                           \
    }                                                                          \
                                                                               \
    return ret;                                                                \
  }

#define GenParseNumber(T) GenParseNumberWithName(T, T)

GenParseNumber(int);
GenParseNumber(size_t);

sds vecstrjoin(Vector *strs, sds sep) {
  return sdsjoin((char **)strs->data, strs->len, sep);
}