#include "cplayground.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

PriorityQueue *new_PriorityQueue(ELEM_COMPARE compare) {
  PriorityQueue *pqueue = xmalloc(sizeof(PriorityQueue));
  pqueue->heap = new_BinaryHeap(compare);
  return pqueue;
}

void pqueue_push(PriorityQueue *pqueue, void *elem) {
  heap_insert(pqueue->heap, elem);
}

void *pqueue_pop(PriorityQueue *pqueue) { return heap_pop(pqueue->heap); }

bool pqueue_empty(PriorityQueue *pqueue) { return heap_empty(pqueue->heap); }