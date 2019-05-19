#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

TEST_CASE(test_parse_num_1, {
  sds src = sdsnew("123");
  Vector* parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject*)(parsed->data[0]);
  assert(get_float_val(obj) == 123);
})


TEST_CASE(test_parse_num_2, {
  sds src = sdsnew("123.456");
  Vector* parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject*)(parsed->data[0]);
  assert(get_float_val(obj) == 123.456);
})

void sexp_test(void) {
  test_parse_num_1();
  test_parse_num_2();

  printf("[sexp_test] All of tests are passed\n");
} 
