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

int mstrcmp(void *lhs, void *rhs) {
  char *lstr = (char *)lhs;
  char *rstr = (char *)rhs;

  int ret = strcmp(lstr, rstr);

  if (ret == 0) {
    return 0;
  }

  if (ret < 0) {
    return -1;
  }

  return 1;
}

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
    avl_insert(tree, sdata[i], INT_TO_VoPTR(v), (ELEM_COMPARE)&mstrcmp);
    avl_print_tree(tree, (ELEM_PRINTER)&str_show, (ELEM_PRINTER)&int_show);
  }

  avl_print_tree(tree, str_show, int_show);

  printf("keys: ");
  Vector *keys = avl_keys(tree);
  VecForeach(keys, elem, { printf("%s ", str_show(elem)); });
  printf("\n");
  printf("values: ");
  Vector *values = avl_values(tree);
  VecForeach(values, elem, { printf("%d ", VoPTR_TO_INT(elem)); });
  printf("\n");
});

TEST_CASE(test_delete, {
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
    int v = i * i;

    printf("-------------------------------------------------------------\n");
    printf("<insert> k: %s, v: %d\n", sdata[i], v);
    avl_insert(tree, sdata[i], INT_TO_VoPTR(v), mstrcmp);
    avl_print_tree(tree, str_show, int_show);
  }

  printf("-------------------------------------------------------------\n");
  avl_print_tree(tree, str_show, int_show);

  printf("keys: ");
  Vector *keys = avl_keys(tree);
  VecForeach(keys, elem, { printf("%s ", str_show(elem)); });
  printf("\n");
  printf("values: ");
  Vector *values = avl_values(tree);
  VecForeach(values, elem, { printf("%d ", VoPTR_TO_INT(elem)); });
  printf("\n");

  for (size_t i = 0; i < data_len; i++) {
    if ((i & 1) == 0) {
      printf("-------------------------------------------------------------\n");
      printf("<delete> k: %s\n", sdata[i]);
      avl_delete(tree, sdata[i], mstrcmp);
      avl_print_tree(tree, str_show, int_show);
    }
  }

  printf("-------------------------------------------------------------\n");
  avl_print_tree(tree, str_show, int_show);

  printf("keys: ");
  keys = avl_keys(tree);
  VecForeach(keys, elem, { printf("%s ", str_show(elem)); });
  printf("\n");
  printf("values: ");
  values = avl_values(tree);
  VecForeach(values, elem, { printf("%d ", VoPTR_TO_INT(elem)); });
  printf("\n");
});

void avl_test(void) {
  test_basic();
  test_delete();
  printf("[avl_test] All of tests are passed\n");
}

#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

int int_cmp(void *lhs, void *rhs) {
  int lv = (intptr_t)lhs;
  int rv = (intptr_t)rhs;

  if (lv == rv) {
    return 0;
  }
  if (lv < rv) {
    return -1;
  }
  return 1;
}

/* Driver program to test above function*/
void avl2_test(void) {
  /*
  AVLTree *tree = new_AVLTree();

  avl_insert(tree, INT_TO_VoPTR(9), INT_TO_VoPTR(9), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(5), INT_TO_VoPTR(5), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(10), INT_TO_VoPTR(10), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(0), INT_TO_VoPTR(0), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(6), INT_TO_VoPTR(6), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(11), INT_TO_VoPTR(11), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(-1), INT_TO_VoPTR(-1), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(1), INT_TO_VoPTR(1), int_cmp);
  avl_insert(tree, INT_TO_VoPTR(2), INT_TO_VoPTR(2), int_cmp);
  avl_print_tree(tree, int_show, int_show);

  avl_delete(tree, INT_TO_VoPTR(10), int_cmp);


  avl_print_tree(tree, int_show, int_show);
  */
}
