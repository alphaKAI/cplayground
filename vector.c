#include "cplayground.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __USE_BOEHM_GC__
#include <gc.h>
#endif

inline Vector *new_vec_with(size_t capacity) {
  Vector *v = xmalloc(sizeof(Vector));
  v->data = xmalloc(sizeof(void *) * capacity);
  v->capacity = capacity;
  v->len = 0;
  return v;
}

inline Vector *new_vec() { return new_vec_with(VECTOR_DEFAULT_CAPACITY); }

void vec_expand(Vector *v, size_t size) {
  if (v->len < size) {
    v->capacity = size;
    v->len = size;
#ifdef __USE_BOEHM_GC__
    v->data = gc_realloc(v->data, sizeof(void *) * v->capacity);
#else
    v->data = realloc(v->data, sizeof(void *) * v->capacity);
#endif
  }
}

inline void vec_push(Vector *v, void *elem) {
  if (v->len == v->capacity) {
    v->capacity *= 2;
#ifdef __USE_BOEHM_GC__
    v->data = gc_realloc(v->data, sizeof(void *) * v->capacity);
#else
    v->data = realloc(v->data, sizeof(void *) * v->capacity);
#endif
  }
  v->data[v->len++] = elem;
}

inline void vec_pushi(Vector *v, int val) { vec_push(v, (void *)(intptr_t)val); }
inline void vec_pushlli(Vector *v, long long int val) { vec_push(v, (void *)val); }

void *vec_pop(Vector *v) {
  assert(v->len);
  return v->data[--v->len];
}

void *vec_last(Vector *v) {
  assert(v->len);
  return v->data[v->len - 1];
}

bool vec_contains(Vector *v, void *elem) {
  for (size_t i = 0; i < v->len; i++) {
    if (v->data[i] == elem) {
      return true;
    }
  }
  return false;
}

bool vec_containss(Vector *v, sds key) {
  for (size_t i = 0; i < v->len; i++) {
    if (!strcmp(v->data[i], key)) {
      return true;
    }
  }
  return false;
}

bool vec_union1(Vector *v, void *elem) {
  if (vec_contains(v, elem)) {
    return false;
  }
  vec_push(v, elem);
  return true;
}

void *vec_get(Vector *v, size_t idx) {
  assert(idx < v->len);
  return v->data[idx];
}

Vector *vec_dup(Vector *v) {
  Vector *vec = new_vec();
  for (size_t i = 0; i < v->len; i++) {
    vec_push(vec, v->data[i]);
  }
  return vec;
}

void vec_append(Vector *v1, Vector *v2) {
  VecForeach(v2, e, { vec_push(v1, e); })
}
