#ifndef __BINARYHEAP_HEADER_INCLUDED__
#define __BINARYHEAP_HEADER_INCLUDED__
#include "cplayground.h"

typedef struct {
  Vector *vec;
  ELEM_COMPARE compare;
} BinaryHeap;

BinaryHeap *new_BinaryHeap(ELEM_COMPARE compare);

void heap_insert(BinaryHeap *heap, void *val);
void *heap_pop(BinaryHeap *heap);
void print_heap(BinaryHeap *heap, ELEM_PRINTER show);
#endif