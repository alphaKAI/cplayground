#include "cplayground.h"
#include <stdio.h>
#include <stdlib.h>
#include <gc.h>

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage: ./%s prog_path\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  gc_init();
  vm_init();
  sds code = readText(sdsnew(argv[1]));
  Vector *parsed = sexp_parse(code);
  Vector *compiled = vm_compile(parsed);
  printf("compiled instructions...\n");
  vm_ins_dump(compiled);
  vm_exec(compiled);
  return 0;
}
