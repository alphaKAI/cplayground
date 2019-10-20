#include "cplayground.h"
#include <stddef.h>
#include <stdlib.h>

RingBuffer *new_RingBuffer(size_t size) {
  RingBuffer *rb = xnew(RingBuffer);
  rb->buffer = xmalloc(size);
  size_t i;
  for (i = 0; i < size; i++) {
    rb->buffer[i] = '\0';
  }
  rb->size = size;
  rb->writer = 0;
  rb->reader = 0;
  return rb;
}

void free_RingBuffer(RingBuffer **rb_ptr) {
  PNULL_CHECK_DEFAULT(rb_ptr);
  PNULL_CHECK_DEFAULT(*rb_ptr);
  RingBuffer *rb = *rb_ptr;
  xfree(&rb->buffer);
  xfree(rb_ptr);
}

size_t readblelen_RingBuffer(RingBuffer *rb) {
  PNULL_CHECK_DEFAULT(rb);

  size_t cursor, line_size = 0;
  for (cursor = rb->reader; cursor != rb->writer && line_size < rb->size;
       cursor++, line_size++)
    ;

  return line_size;
}

char *read_RingBuffer(RingBuffer *rb) {
  PNULL_CHECK_DEFAULT(rb);
  size_t line_size = 0;
  size_t cursor;

  for (cursor = rb->reader; cursor != rb->writer && line_size < rb->size;
       cursor++, line_size++)
    ;
  char *ret = xmalloc(line_size + 1);
  size_t i;
  for (i = 0; i < line_size; i++) {
    ret[i] = rb->buffer[(rb->reader + i) % rb->size];
  }

  ret[line_size] = '\0';
  rb->reader = cursor;
  return ret;
}

void write_RingBuffer(RingBuffer *rb, char *str, size_t size) {
  PNULL_CHECK_DEFAULT(rb);

  size_t cursor = rb->writer;
  size_t wrote = 0;
  for (; wrote < size; cursor++, wrote++) {
    rb->buffer[cursor % rb->size] = str[wrote];
  }
  rb->writer = cursor;
  if (rb->size < rb->writer - rb->reader) {
    rb->reader = rb->writer - rb->size;
  }
}
