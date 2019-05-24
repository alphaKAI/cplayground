#include "cplayground.h"
#include "sds/sds.h"
#include "tests.h"
#include <assert.h>
#include <stddef.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define INT_TO_VoPTR(i) ((void *)(intptr_t)i)
#define VoPTR_TO_INT(ptr) ((int)(intptr_t)ptr)

void int_free(void *_ __attribute__((unused))) {}

static int data[] = {2, 3, 4, 1, 1, 4, 2, 1, 5, 6, 3, 2,
                     3, 4, 2, 3, 7, 5, 6, 7, 5, 1, 1, 4};
static size_t data_len = ARRAY_LEN(data);

TEST_CASE(test_push1, {
  Stack *stack = new_Stack();
  for (size_t i = 0; i < data_len; i++) {
    push_Stack(stack, INT_TO_VoPTR(data[i]));
  }

  assert(stack->elem_count == data_len);

  free_Stack(&stack, int_free);
});

TEST_CASE(test_pop1, {
  Stack *stack = new_Stack();
  for (size_t i = 0; i < data_len; i++) {
    push_Stack(stack, INT_TO_VoPTR(data[i]));
  }

  assert(stack->elem_count == data_len);

  assert(data[data_len - 1] == VoPTR_TO_INT(peek_Stack(stack)));

  for (size_t i = 0; i < data_len; i++) {
    int v = VoPTR_TO_INT(pop_Stack(stack));
    assert(v == data[data_len - i - 1]);
  }

  assert(stack->elem_count == 0);

  free_Stack(&stack, int_free);
});

void stack_test(void) {
  test_push1();
  test_pop1();

  printf("[stack_test] All of tests are passed\n");
}