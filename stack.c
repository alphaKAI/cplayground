#include "cplayground.h"
#include <stdio.h>
#include <stdlib.h>

Stack *new_Stack() {
  Stack *stack = xmalloc(sizeof(Stack));
  stack->data = new_vec_with(STACK_DEFAULT_CAPACITY);
  stack->elem_count = 0;

  return stack;
}

void free_Stack(Stack **s_ptr, S_DATA_FREE free_func) {
  Stack *stack = *s_ptr;

  for (size_t i = 0; i < stack->elem_count; i++) {
    free_func(stack->data->data[i]);
  }

  xfree(s_ptr);
}

void push_Stack(Stack *stack, void *val) {
  stack->elem_count++;
  vec_push(stack->data, val);
}

void *pop_Stack(Stack *stack) {
  if (stack->elem_count == 0) {
    fprintf(stderr, "<pop_Stack> Stack is empty!\n");
    exit(EXIT_FAILURE);
  }
  stack->elem_count--;
  return vec_pop(stack->data);
}

void *peek_Stack(Stack *stack) {
  if (stack->elem_count == 0) {
    fprintf(stderr, "<peek_Stack> Stack is empty!\n");
    exit(EXIT_FAILURE);
  }

  return stack->data->data[stack->elem_count - 1];
}

void print_Stack(Stack *stack, S_DATA_SHOW show) {
  printf("[");
  for (size_t i = 0; i < stack->elem_count; i++) {
    if (i > 0) {
      printf(", ");
    }
    printf("%s", show(stack->data->data[i]));
  }
  printf("]\n");
}