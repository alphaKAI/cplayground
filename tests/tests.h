#ifndef __TESTS_HEADER_INCLUDED__
#define __TESTS_HEADER_INCLUDED__
#include <stdio.h>

#define TEST_CASE(test_name, test_body)                                        \
  static void test_name(void) {                                                \
    test_body;                                                                 \
    printf("[Test - OK] " #test_name "\n");                                    \
  }

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

void binarytree_test(void);
void queue_test(void);
void functional_test(void);
void avl_test(void);
void util_test(void);
void sexp_test(void);
void stack_test(void);
void heap_test(void);
#endif
