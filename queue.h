#ifndef __QUEUE_HEADER_INCLUDED__
#define __QUEUE_HEADER_INCLUDED__

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  Vector *data;
  size_t elem_count;
  size_t head;
  size_t tail;
} Queue;

typedef void (*Q_DATA_FREE)();
typedef sds (*Q_DATA_SHOW)(void *);

Queue *new_Queue(void);
void free_Queue(Queue **q_ptr, Q_DATA_FREE free_func);
void enqueue_Queue(Queue *queue, void *val);
void *dequeue_Queue(Queue *queue);
void print_Queue(Queue *queue, Q_DATA_SHOW show);

#endif
