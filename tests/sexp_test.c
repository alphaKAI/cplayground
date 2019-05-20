#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST_CASE(test_parse_num_1, {
  sds src = sdsnew("123");
  ParseResult parsed = sexp_parse_expr(src);
  assert(get_float_val(parsed.parse_result) == 123);
});

TEST_CASE(test_parse_num_2, {
  sds src = sdsnew("123.456");
  ParseResult parsed = sexp_parse_expr(src);
  assert(get_float_val(parsed.parse_result) == 123.456);
});

TEST_CASE(test_parse_symbol_1, {
  sds src = sdsnew("abc");
  ParseResult parsed = sexp_parse_expr(src);
  assert(sdscmp(get_symbol_val(parsed.parse_result), src) == 0);
});

TEST_CASE(test_parse_symbol_2, {
  sds src = sdsnew("$abc#");
  ParseResult parsed = sexp_parse_expr(src);
  assert(sdscmp(get_symbol_val(parsed.parse_result), src) == 0);
});

TEST_CASE(test_parse_string_1, {
  sds src = sdsnew("\"abc\"");
  ParseResult parsed = sexp_parse_expr(src);
  assert(strcmp(get_string_val(parsed.parse_result), "abc") == 0);
});

TEST_CASE(test_parse_string_2, {
  sds src = sdsnew("\"\"");
  ParseResult parsed = sexp_parse_expr(src);
  assert(strcmp(get_string_val(parsed.parse_result), "") == 0);
});

TEST_CASE(test_parse_list_1, {
  sds src = sdsnew("(1 2 3)");
  ParseResult parsed = sexp_parse_expr(src);
  Vector *v = new_vec();
  vec_push(v, new_SexpObject_float(1));
  vec_push(v, new_SexpObject_float(2));
  vec_push(v, new_SexpObject_float(3));
  SexpObject *eobj = new_SexpObject_list(v);
  assert(equal_SexpObjects(parsed.parse_result, eobj));
});

TEST_CASE(test_parse_list_2, {
  sds src = sdsnew("(hello \"world\")");
  ParseResult parsed = sexp_parse_expr(src);
  Vector *v = new_vec();
  vec_push(v, new_SexpObject_symbol(sdsnew("hello")));
  vec_push(v, new_SexpObject_string(sdsnew("world")));
  SexpObject *eobj = new_SexpObject_list(v);
  assert(equal_SexpObjects(parsed.parse_result, eobj));
});

TEST_CASE(test_parse_quote_1, {
  sds src = sdsnew("'(1 2 3)");
  ParseResult parsed = sexp_parse_expr(src);
  Vector *v = new_vec();
  vec_push(v, new_SexpObject_float(1));
  vec_push(v, new_SexpObject_float(2));
  vec_push(v, new_SexpObject_float(3));
  SexpObject *eobj = new_SexpObject_list(v);
  assert(!equal_SexpObjects(parsed.parse_result, eobj));
  assert(equal_SexpObjects(parsed.parse_result, new_SexpObject_quote(eobj)));
})

TEST_CASE(test_parse_multi_line_1, {
  sds src = sdsnew("\
'(1 2 3)\
'(4 5 6)");

  Vector *parsed_all = sexp_parse(src);
  SexpObject *obj;
  SexpObject *eobj;
  Vector *v;

  obj = parsed_all->data[0];

  v = new_vec();
  vec_push(v, new_SexpObject_float(1));
  vec_push(v, new_SexpObject_float(2));
  vec_push(v, new_SexpObject_float(3));
  eobj = new_SexpObject_list(v);
  assert(!equal_SexpObjects(obj, eobj));
  assert(equal_SexpObjects(obj, new_SexpObject_quote(eobj)));

  obj = parsed_all->data[1];

  v = new_vec();
  vec_push(v, new_SexpObject_float(4));
  vec_push(v, new_SexpObject_float(5));
  vec_push(v, new_SexpObject_float(6));
  eobj = new_SexpObject_list(v);
  assert(!equal_SexpObjects(obj, eobj));
  assert(equal_SexpObjects(obj, new_SexpObject_quote(eobj)));
});

TEST_CASE(test_parse_multi_line_2, {
  sds src = sdsnew("\
'(1 2 3)\
'hello");

  Vector *parsed_all = sexp_parse(src);
  SexpObject *obj;
  SexpObject *eobj;
  Vector *v;

  obj = parsed_all->data[0];

  v = new_vec();
  vec_push(v, new_SexpObject_float(1));
  vec_push(v, new_SexpObject_float(2));
  vec_push(v, new_SexpObject_float(3));
  eobj = new_SexpObject_list(v);
  assert(!equal_SexpObjects(obj, eobj));
  assert(equal_SexpObjects(obj, new_SexpObject_quote(eobj)));

  obj = parsed_all->data[1];

  eobj = new_SexpObject_symbol(sdsnew("hello"));
  assert(!equal_SexpObjects(obj, eobj));
  assert(equal_SexpObjects(obj, new_SexpObject_quote(eobj)));
});

void sexp_test(void) {
  test_parse_num_1();
  test_parse_num_2();
  test_parse_symbol_1();
  test_parse_symbol_2();
  test_parse_string_1();
  test_parse_string_2();
  test_parse_list_1();
  test_parse_list_2();
  test_parse_quote_1();
  test_parse_multi_line_1();
  test_parse_multi_line_2();

  printf("[sexp_test] All of tests are passed\n");
}
