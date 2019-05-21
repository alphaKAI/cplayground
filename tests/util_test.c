#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <stdio.h>

TEST_CASE(test_parseDouble_1, {
  sds str = sdsnew("123");

  assert(parseDouble(str) == 123);
})

TEST_CASE(test_parseDouble_2, {
  sds str = sdsnew("123.456");
  double v = parseDouble(str);

  printf("v: %f\n", v);

  assert(v == 123.456);
})

TEST_CASE(test_read_text, {
  sds file_name = sdsnew("util.c");
  sds text = readText(file_name);

  printf("%s", text);
})

void util_test(void) {
  test_parseDouble_1();
  test_parseDouble_2();
  test_read_text();

  printf("[util_test] All of tests are passed\n");
}
