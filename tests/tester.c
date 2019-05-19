#include "tests.h"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  binarytree_test();
  queue_test();
  functional_test();
  util_test();
  sexp_test();

  return 0;
}
