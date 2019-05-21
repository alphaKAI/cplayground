#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stdio.h>
#include <assert.h>

TEST_CASE(test_parse_double_1, {
  sds str = sdsnew("123");

  assert(parse_double(str) == 123);
})

TEST_CASE(test_parse_double_2, {
  sds str = sdsnew("123.456");
  double v = parse_double(str);

  printf("v: %f\n", v);

  assert(v == 123.456);
})

TEST_CASE(test_read_text, {
  sds file_name = sdsnew("util.c");
  sds text = read_text(file_name);

  printf("%s", text);
})



void util_test(void) {
  test_parse_double_1();
  test_parse_double_2();
  test_read_text();

  printf("[util_test] All of tests are passed\n");
}
