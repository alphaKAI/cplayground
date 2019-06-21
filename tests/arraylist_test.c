#include "cplayground.h"
#include "tests.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int val;
  ArrayListNode node;
} IntContainter;

IntContainter *new_IC(int val) {
  IntContainter *ic = xnew(IntContainter);
  ic->val = val;
  init_ArrayListNode(&ic->node);
  return ic;
}

#define SHOW_IC_STR_MAX 20

char *show_IC(IntContainter *ic) {
  PNULL_CHECK_DEFAULT(ic);
  char *buf = xmalloc(20);
  sprintf(buf, "%d", ic->val);
  return buf;
}

#define N 20

void arraylist_test(void) {
  IntContainter **ics = xnewN(IntContainter *, N);
  for (size_t i = 0; i < N; i++) {
    ics[i] = new_IC(i);
  }

  ArrayList *list = new_ArrayList(N);

  for (size_t i = 0; i < N; i += 2) {
    append_ArrayList(list, &ics[i]->node);
  }

  for (size_t i = 1; i < N; i += 2) {
    append_ArrayList(list, &ics[i]->node);
  }

  print_ArrayList(IntContainter, list, node, show_IC);
}
