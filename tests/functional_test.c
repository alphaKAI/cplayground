#include "cplayground.h"
#include "tests.h"
#include <assert.h>

void *square(void *vi) {
  long v = (intptr_t)vi;
  return (void *)(intptr_t)(v * v);
}

#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

TEST_CASE(basic_test, {
  void *ret = PIPE(INT_TO_VoPTR(4), square, square);
  assert(VoPTR_TO_INT(ret) == 256);
})

void functional_test(void) {
  basic_test();
  printf("[functional_test] All of tests are passed\n");
}