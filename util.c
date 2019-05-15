#include "cplayground.h"
#include <stdio.h>
#include <stdlib.h>

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
