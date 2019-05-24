#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

static sds int_show(void *val) {
  return sdscatprintf(sdsempty(), "%ld", (intptr_t)val);
}

static int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
static size_t data_len = ARRAY_LEN(data);

// TODO: Improve test

sds str_show(void *ptr) { return (sds)ptr; }

TEST_CASE(test_basic, {
  AVLTree *tree = new_AVLTree();
  sds sdata[9];
  sdata[0] = sdsnew("a");
  sdata[1] = sdsnew("b");
  sdata[2] = sdsnew("c");
  sdata[3] = sdsnew("d");
  sdata[4] = sdsnew("e");
  sdata[5] = sdsnew("f");
  sdata[6] = sdsnew("g");
  sdata[7] = sdsnew("h");
  sdata[8] = sdsnew("i");

  for (size_t i = 0; i < data_len; i++) {
    int k = data[i];
    int v = k * k;

    printf("<insert> k: %d, v: %d\n", k, v);
    avl_insert(tree, sdata[i], INT_TO_VoPTR(v), (ELEM_COMPARE)&strcmp);
    print_tree(tree, (ELEM_PRINTER)&str_show, (ELEM_PRINTER)&int_show);
  }

  print_tree(tree, str_show, int_show);

  printf("keys: ");
  Vector *keys = avl_keys(tree);
  VecForeach(keys, elem, { printf("%s ", str_show(elem)); });
  printf("\n");
  printf("values: ");
  Vector *values = avl_values(tree);
  VecForeach(values, elem, { printf("%d ", VoPTR_TO_INT(elem)); });
  printf("\n");
});

void avl_test(void) {
  test_basic();
  printf("[avl_test] All of tests are passed\n");
}