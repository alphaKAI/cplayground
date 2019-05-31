#include "cplayground.h"
#include <stdio.h>
#include <stdlib.h>

Queue *new_Queue(void) {
  Queue *queue = xnew(Queue);
  queue->data = new_vec();
  queue->elem_count = 0;
  queue->head = 0;
  queue->tail = 0;
  return queue;
}

void free_Queue(Queue **q_ptr, Q_DATA_FREE free_func) {
  if (q_ptr == NULL || *q_ptr == NULL) {
    fprintf(stderr, "free_Queue got a NULL pointer\n");
    exit(EXIT_FAILURE);
  }

  for (size_t i = (*q_ptr)->head;
       i < (*q_ptr)->tail && 0 < (*q_ptr)->elem_count; i++) {
    free_func((*q_ptr)->data[i]);
  }

  xfree((void *)&(*q_ptr)->data);
  xfree((void *)q_ptr);
  *q_ptr = NULL;
}

void enqueue_Queue(Queue *queue, void *val) {
  queue->elem_count++;
  queue->tail++;
  vec_push(queue->data, val);
}

void *dequeue_Queue(Queue *queue) {
  if (queue->elem_count == 0) {
    fprintf(stderr, "Can't dequeue from empty queue\n");
    exit(EXIT_FAILURE);
  }

  queue->elem_count--;
  return vec_get(queue->data, queue->head++);
}

void print_Queue(Queue *queue, Q_DATA_SHOW show) {
  printf("len : %ld ", queue->elem_count);
  printf("[");

  for (size_t i = queue->head; i < queue->tail && 0 < queue->elem_count; i++) {
    if (i > queue->head) {
      printf(", ");
    }

    printf("%s", show(vec_get(queue->data, i)));
  }

  printf("]\n");
}