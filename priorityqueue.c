#include "cplayground.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  BinaryHeap *heap;
} PriorityQueue;

PriorityQueue *new_PriorityQueue(ELEM_COMPARE compare) {
  PriorityQueue *pqueue = xmalloc(sizeof(PriorityQueue));
  pqueue->heap = new_BinaryHeap(compare);
  return pqueue;
}

void pqueue_push(PriorityQueue *pqueue, void *elem) {
  heap_insert(pqueue->heap, elem);
}

void *pqueue_pop(PriorityQueue *pqueue) { return heap_pop(pqueue->heap); }