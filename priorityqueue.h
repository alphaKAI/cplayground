#ifndef __PRIORITYQUEUE_HEADER_INCLUDED__
#define __PRIORITYQUEUE_HEADER_INCLUDED__
#include "cplayground.h"

typedef struct {
  BinaryHeap *heap;
} PriorityQueue;

PriorityQueue *new_PriorityQueue(ELEM_COMPARE compare);
void pqueue_push(PriorityQueue *pqueue, void *elem);
void *pqueue_pop(PriorityQueue *pqueue);
bool pqueue_empty(PriorityQueue *pqueue);
#endif