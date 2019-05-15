#ifndef __FUNCTIONAL_HEADER_INCLUDED__

#include "cplayground.h"

#define PIPE(val, ...) pipegen(val, PIPE_ARG_MAKE(__VA_ARGS__))
#define PIPE_ARG_MAKE(...) PIPE_ARG_MAKE2(((void *[]){__VA_ARGS__}))
#define PIPE_ARG_MAKE2(ARRAY) ARRAY, sizeof(ARRAY) / sizeof((ARRAY)[0])

typedef void *(*ANY_FUNCTION)(void *);

void *pipegen(void *val, void **funcs, size_t len);
#endif