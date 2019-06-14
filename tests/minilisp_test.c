#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void minilisp_test_1(void) {
  Vector *v_ins = new_vec();
  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(new_SexpObject_float(1)));
  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(new_SexpObject_float(4)));
  vec_pushi(v_ins, OpAdd);
  vec_pushi(v_ins, OpPrintln);

  Vector *f1_ins = new_vec();
  vec_pushi(f1_ins, OpPush);
  vec_push(f1_ins,
           new_VMValueWithValue(new_SexpObject_string(sdsnew("Enter f1!"))));
  vec_pushi(f1_ins, OpPrintln);
  vec_pushi(f1_ins, OpPush);
  vec_push(f1_ins, new_VMValueWithValue(new_SexpObject_float(5)));
  vec_pushi(f1_ins, OpPush);
  vec_push(f1_ins, new_VMValueWithValue(new_SexpObject_float(3)));
  vec_pushi(f1_ins, OpMul);
  vec_pushi(f1_ins, OpPrintln);
  vec_pushi(f1_ins, OpReturn);

  VMFunction *f1 = new_VMFunction(sdsnew("f1"), f1_ins, NULL);
  vec_pushi(v_ins, OpFuncDef);
  vec_push(v_ins, new_VMValueWithFunc(f1));
  vec_pushi(v_ins, OpDumpEnv);
  vec_pushi(v_ins, OpCall);
  vec_push(v_ins, sdsnew("f1"));
  vec_pushi(v_ins, 0);
  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(
                      new_SexpObject_string(sdsnew("Return from f1!"))));
  vec_pushi(v_ins, OpPrintln);

  Vector *f2_ins = new_vec();
  vec_pushi(f1_ins, OpPush);
  vec_push(f1_ins,
           new_VMValueWithValue(new_SexpObject_string(sdsnew("Enter f2!"))));
  vec_pushi(f2_ins, OpPush);
  vec_push(f2_ins, new_VMValueWithValue(new_SexpObject_float(3)));
  vec_pushi(f2_ins, OpVarDef);
  vec_push(f2_ins, sdsnew("x"));
  vec_pushi(f2_ins, OpPush);
  vec_push(f2_ins, new_VMValueWithValue(new_SexpObject_float(8)));
  vec_pushi(f2_ins, OpVarDef);
  vec_push(f2_ins, sdsnew("y"));
  vec_pushi(f2_ins, OpGetVar);
  vec_push(f2_ins, sdsnew("x"));
  vec_pushi(f2_ins, OpGetVar);
  vec_push(f2_ins, sdsnew("y"));
  vec_pushi(f2_ins, OpMul);
  vec_pushi(f2_ins, OpPrintln);

  VMFunction *f2 = new_VMFunction(sdsnew("f2"), f2_ins, NULL);
  vec_pushi(v_ins, OpFuncDef);
  vec_push(v_ins, new_VMValueWithFunc(f2));
  vec_pushi(v_ins, OpDumpEnv);
  vec_pushi(v_ins, OpCall);
  vec_push(v_ins, sdsnew("f2"));
  vec_pushi(v_ins, 0);
  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(
                      new_SexpObject_string(sdsnew("Return from f2!"))));
  vec_pushi(v_ins, OpPrintln);

  Vector *tmp = new_vec();
  vec_push(tmp, new_VMValueWithValue(new_SexpObject_float(1)));
  vec_push(tmp, new_VMValueWithValue(new_SexpObject_bool(true)));
  vec_push(tmp, new_VMValueWithValue(new_SexpObject_string(sdsnew("Hello"))));
  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(new_SexpObject_list(tmp)));
  vec_pushi(v_ins, OpPrintln);

  vec_pushi(v_ins, OpPush);
  vec_push(v_ins, new_VMValueWithValue(new_SexpObject_string(sdsnew("Hello"))));
  vec_pushi(v_ins, OpCall);
  vec_push(v_ins, sdsnew("println"));
  vec_pushi(v_ins, 1);

  for (size_t i = 0; i < 3; i++) {
    vec_pushi(v_ins, OpPush);
    vec_push(v_ins, new_VMValueWithValue(new_SexpObject_float(i)));
  }
  vec_pushi(v_ins, OpMakeList);
  vec_pushi(v_ins, 3);
  vec_pushi(v_ins, OpPrintln);

  SexpObject *ret = vm_exec(v_ins);

  vm_ins_dump(v_ins);

  sds codes[] = {
      sdsnew("1"),
      sdsnew("true"),
      sdsnew("\"abc\""),
      sdsnew("foo"),
      sdsnew("(println \"Hello, world!\")"),
      sdsnew("'(1 2 3)"),
      sdsnew("(def-fun sq (x) (* x x))"),
      sdsnew("(== 1 1)"),
      sdsnew("(if (== 1 1) (println \"1 == 1\") (println \"1 != 1 !?!?\"))")};

  for (size_t i = 0; i < ARRAY_LEN(codes); i++) {
    printf("--------------------------------------------------------\n");
    sds code = codes[i];
    printf("code: %s\n", code);
    Vector *parsed = sexp_parse(code);
    Vector *ins = vm_compile(parsed);
    printf("compiled instructions...\n");
    vm_ins_dump(ins);
  }
}

void minilisp_test_2(void) {
  vm_init();
  sds code = sdsnew("\t(println \"Hello, world\")\n\
\t(def-var x 4)\n\
\t(def-fun sq (x) (* x x))\n\
\t(println (* x (sq 3)))\n\
\t(println (== 1 1))\n\
\t(def-fun add (x y) (+ x y))\n\
\t(println (add 1 2))\n\
\t(print \"x: \")\n\
\t(println x)\n\
\t(println (/ 9 3))\n\
\t(if (== 1 2) (println (+ 1 (* 2 (/ 9 3)))) (println \"false\"))");
  printf("code:\n%s\n", code);
  Vector *parsed = sexp_parse(code);
  Vector *compiled = vm_compile(parsed);
  vm_ins_dump(compiled);
  vm_exec(compiled);
}

void minilisp_test_3(void) {
  vm_init();
  sds code = readText("tests/hw.lsp");
  printf("code:\n");
  printf("%s", code);
  Vector *parsed = sexp_parse(code);
  Vector *compiled = vm_compile(parsed);
  printf("compiled instructions...\n");
  vm_ins_dump(compiled);
  vm_exec(compiled);
}

void minilisp_test_4(void) {
  vm_init();
  sds code = readText("tests/fib.lsp");
  printf("code:\n");
  printf("%s", code);
  Vector *parsed = sexp_parse(code);
  Vector *compiled = vm_compile(parsed);
  printf("compiled instructions...\n");
  vm_ins_dump(compiled);
  vm_exec(compiled);
}

void minilisp_test(void) {
  minilisp_test_1();
  minilisp_test_2();
  minilisp_test_3();
  minilisp_test_4();
}