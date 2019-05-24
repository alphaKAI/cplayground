#ifndef __TESTS_HEADER_INCLUDED__
#define __TESTS_HEADER_INCLUDED__
#include <stdio.h>

#define TEST_CASE(test_name, test_body)                                        \
  static void test_name(void) {                                                \
    test_body;                                                                 \
    printf("[Test - OK] " #test_name "\n");                                    \
  }

void binarytree_test(void);
void queue_test(void);
void functional_test(void);
void avl_test(void);
void util_test(void);
void sexp_test(void);
void stack_test(void);
#endif
