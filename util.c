#include "cplayground.h"
#include "sds/sds.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __USE_BOEHM_GC__
#include <gc.h>
#endif

void *xmalloc(size_t size) {
#ifdef __USE_BOEHM_GC__
  void *ptr = GC_malloc(size);
#else
  void *ptr = malloc(size);
#endif
  if (ptr == NULL) {
    fprintf(stderr, "Failed to allocate memory\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void xfreeImpl(void **p_ptr) {
#ifdef __USE_BOEHM_GC__
  GC_free(*p_ptr);
#else
  if (p_ptr == NULL || *p_ptr == NULL) {
    fprintf(stderr, "Given pointer is NULL");
    exit(EXIT_FAILURE);
  }

  free(*p_ptr);
  *p_ptr = NULL;
#endif
}

static double dpow(double n, size_t p) {
  double t = 1.;
  for (size_t i = 0; i < p; i++) {
    t *= n;
  }
  return t;
}

#define ASCII_NUM_TO_INT(i) (i - '0')

double parseDouble(sds str) {
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

#define min(a, b) (a < b ? a : b)

size_t checkedSizeSub(size_t a, size_t b) {
  if (a < b) {
    fprintf(stderr, "detect underflow\n");
    exit(EXIT_FAILURE);
  } else {
    return a - b;
  }
}

size_t checkedSizeAdd(size_t a, size_t b) {
  if (SIZE_MAX - b < a) {
    fprintf(stderr, "detect overflow\n");
    exit(EXIT_FAILURE);
  } else {
    return a + b;
  }
}

static SizedData readImpl(const sds file_path, size_t upTo) {
  size_t minInitialAlloc = 1024 * 4;
  size_t maxInitialAlloc = SIZE_MAX / 2;
  size_t sizeIncrement = 1024 * 16;
  size_t maxSlackMemoryAllowed = 1024;

  int fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file - %s\n", file_path);
    exit(EXIT_FAILURE);
  }

  struct stat fs;
  if (fstat(fd, &fs) < 0) {
    fprintf(stderr, "Error fstat\n");
    exit(EXIT_FAILURE);
  }

  const size_t initialAlloc = min(
      upTo, (size_t)(fs.st_size ? min((size_t)fs.st_size + 1, maxInitialAlloc)
                                : minInitialAlloc));

  void *result = xmalloc(initialAlloc);
  size_t result_len = initialAlloc;
  size_t size = 0;

  for (;;) {
    ssize_t actual =
        read(fd, result + size, checkedSizeSub(min(result_len, upTo), size));
    if (actual == -1) {
      fprintf(stderr, "Error at readImpl<file_path: %s>\n", file_path);
      exit(EXIT_FAILURE);
    }

    if (actual == 0) {
      break;
    }
    size = checkedSizeAdd(size, actual);
    if (size >= upTo) {
      break;
    }
    if (size < result_len) {
      break;
    }
    const size_t newAlloc = checkedSizeAdd(size, sizeIncrement);

    result = realloc(result, newAlloc);
    result_len = newAlloc;
  }

  close(fd);

  SizedData ret;

  if (checkedSizeSub(result_len, size) >= maxSlackMemoryAllowed) {
    ret.data = realloc(result, size);
    ret.size = size;
  } else {
    ret.data = result;
    ret.size = size;
  }

  return ret;
}

static SizedData readFile(const sds file_path) {
  return readImpl(file_path, SIZE_MAX);
}

sds readText(const sds file_path) {
  SizedData sdata = readFile(file_path);
  ((char *)sdata.data)[sdata.size] = '\0';
  return sdsnew((char *)sdata.data);
}

void write_llis_to_file(const char *file_name, Vector *vec) {
  FILE *fp = fopen(file_name, "wb");

  if (fp == NULL) {
    fprintf(stderr, "failed to open %s\n", file_name);
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < vec->len; i++) {
    long long int lli = (long long int)vec->data[i];
    fwrite(&lli, sizeof(long long int), 1, fp);
  }

  fclose(fp);
}

Vector *read_file_from_llis(const char *file_name) {
  FILE *fp = fopen(file_name, "rb");
  Vector *ret = new_vec();

  if (fp == NULL) {
    fprintf(stderr, "failed to open %s\n", file_name);
    exit(EXIT_FAILURE);
  }

  long long int lli;
  size_t size;
  while ((size = fread(&lli, sizeof(long long int), 1, fp)) != 0) {
    vec_pushlli(ret, lli);
  }

  fclose(fp);

  return ret;
}
