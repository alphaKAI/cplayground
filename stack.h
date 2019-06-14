#ifndef __STACK_HEADER_INCLUDED__
#define __STACK_HEADER_INCLUDED__

#include "cplayground.h"
#include <stddef.h>
#include <stdbool.h>

#define STACK_DEFAULT_CAPACITY 128

typedef struct {
  Vector *data;
  size_t elem_count;
} Stack;

typedef void (*S_DATA_FREE)();
typedef sds (*S_DATA_SHOW)(void *);

Stack *new_Stack();
void free_Stack(Stack **s_ptr, S_DATA_FREE free_func);
void push_Stack(Stack *stack, void *val);
void *pop_Stack(Stack *stack);
void *peek_Stack(Stack *stack);
bool isempty_Stack(Stack *stack);
void print_Stack(Stack *stack, S_DATA_SHOW show);

#endif
