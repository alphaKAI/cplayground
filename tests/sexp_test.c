#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_CASE(test_parse_num_1, {
  sds src = sdsnew("123");
  Vector *parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject *)(parsed->data[0]);
  assert(get_float_val(obj) == 123);
})

TEST_CASE(test_parse_num_2, {
  sds src = sdsnew("123.456");
  Vector *parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject *)(parsed->data[0]);
  assert(get_float_val(obj) == 123.456);
})

TEST_CASE(test_parse_str_1, {
  sds src = sdsnew("\"hello, world\"");
  Vector *parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject *)parsed->data[0];
  assert(strcmp(get_string_val(obj), "hello, world") == 0);
})

TEST_CASE(test_parse_str_2, {
  sds src = sdsnew("(hello)");
  Vector *parsed = sexp_parse(src);
  SexpObject *obj = (SexpObject *)parsed->data[0];
  Vector *expected_v = new_vec();
  vec_push(expected_v, new_SexpObject_string(sdsnew("hello")));
  SexpObject *expected = new_SexpObject_array(expected_v);
  assert(equal_SexpObjects(obj, expected));
})

void sexp_test(void) {
  test_parse_num_1();
  test_parse_num_2();
  test_parse_str_1();
  test_parse_str_2();

  printf("[sexp_test] All of tests are passed\n");
}
