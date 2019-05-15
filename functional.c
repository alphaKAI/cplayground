#include "cplayground.h"

void *pipegen(void *val, void **funcs, size_t len) {
  for (size_t i = 0; i < len; i++) {
    val = ((ANY_FUNCTION)funcs[i])(val);
  }

  return val;
}
