#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

static sds int_show(void *val) {
  return sdscatprintf(sdsempty(), "%ld", (intptr_t)val);
}

static void int_free(void *_) {}

static int data[] = {
    2, 3, 4, 1, 1, 4, 2, 1, 5, 6, 3, 2, 3, 4, 2, 3, 7, 5, 6, 7, 5, 1, 1, 4,
    2, 1, 5, 6, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2,
    3, 2, 3, 4, 1, 1, 4, 2, 1, 5, 6, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3,
    4, 1, 1, 4, 2, 1, 5, 6, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3, 4, 1, 1,
    4, 2, 1, 5, 6, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3, 4, 1, 1, 4, 2, 1,
    5, 6, 3, 2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3, 4, 1, 1, 4, 2, 1, 5, 6, 3,
    2, 3, 4, 6, 2, 3, 5, 4, 2, 3, 2, 3, 4, 1, 1, 4, 2, 1, 5, 6, 3, 2, 3, 4,
};

// TODO: Improve test

TEST_CASE(test_basic, {
  size_t data_len = ARRAY_LEN(data);

  Queue *queue = new_Queue(sizeof(int));

  for (size_t i = 0; i < data_len; i++) {
    printf("QUEUE: ");
    print_Queue(queue, int_show);
    enqueue_Queue(queue, INT_TO_VoPTR(data[i]));
  }

  printf("QUEUE: ");
  print_Queue(queue, int_show);

  for (size_t i = 0; i < CLEANUP_THRESHOLD + 2; i++) {
    dequeue_Queue(queue, int_free);
  }

  free_Queue(&queue, int_free);
})

void binarytree_test(void) {
  test_basic();
  printf("[binarytree_test] All of tests are passed\n");
}
