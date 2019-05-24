#include "cplayground.h"
#include <gc/gc.h>

void gc_init() { GC_INIT(); }

void *gc_malloc(size_t size) { return GC_malloc(size); }

void *gc_realloc(void *ptr, size_t size) { return GC_realloc(ptr, size); }

void gc_free(void *ptr) { GC_free(ptr); }
