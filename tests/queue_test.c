#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

static int int_cmp(void *lhs, void *rhs) {
  int vl = (intptr_t)lhs;
  int vr = (intptr_t)rhs;

  if (vl < vr) {
    return -1;
  }

  if (vl == vr) {
    return 0;
  }

  return 1;
}

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
static size_t data_len = ARRAY_LEN(data);

// TODO: Improve test

TEST_CASE(test_basic, {
  BinaryTree *root = new_BinaryTree(INT_TO_VoPTR(1));

  for (size_t i = 0; i < data_len; i++) {
    insert(root, INT_TO_VoPTR(data[i]), int_cmp);
  }

  printf("%s\n", stringify(root, int_show));

  free_BinaryTree(root, int_free);
})

void queue_test(void) {
  test_basic();
  printf("[queue_test] All of tests are passed\n");
}