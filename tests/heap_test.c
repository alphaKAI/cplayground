#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sds int_show(void *val) {
  return sdscatprintf(sdsempty(), "%ld", (intptr_t)val);
}

static int int_cmp(void *lhs, void *rhs) {
  int lv = (intptr_t)lhs;
  int rv = (intptr_t)rhs;

  if (lv > rv) {
    return 1;
  }
  return -1;
}

int datas[] = {11, 5, 8, 3, 4, 15};
TEST_CASE(basic_test, {
  BinaryHeap *heap = new_BinaryHeap(int_cmp);

  for (size_t i = 0; i < ARRAY_LEN(datas); i++) {
    heap_insert(heap, INT_TO_VoPTR(datas[i]));
  }

  print_heap(heap, int_show);

  for (size_t i = 0; i < ARRAY_LEN(datas); i++) {
    printf("-> %d\n", VoPTR_TO_INT(heap_pop(heap)));
  }
});

void heap_test(void) {
  basic_test();

  printf("[sexp_test] All of tests are passed\n");
}