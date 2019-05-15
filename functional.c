#include "cplayground.h"

#define PIPE(val, ...) pipegen(val, PIPE_ARG_MAKE(__VA_ARGS__))
#define PIPE_ARG_MAKE(...) PIPE_ARG_MAKE2(((void *[]){__VA_ARGS__}))
#define PIPE_ARG_MAKE2(ARRAY) ARRAY, sizeof(ARRAY) / sizeof((ARRAY)[0])

void *pipegen(void *val, void **funcs, size_t len) {
  for (size_t i = 0; i < len; i++) {
    val = ((ANY_FUNCTION)funcs[i])(val);
  }

  return val;
}
