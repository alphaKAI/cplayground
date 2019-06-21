#include "cplayground.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

ArrayListNode *new_ArrayListNode(void) {
  ArrayListNode *node = xnew(ArrayListNode);
  return node;
}

ArrayList *new_ArrayList(ssize_t list_size) {
  ArrayList *list = xnew(ArrayList);

  list->list = xnewN(ArrayListNode *, list_size);

  list->list_size = list_size;
  list->head = -1;
  list->tail = -1;

  return list;
}

void append_ArrayList(ArrayList *this, ArrayListNode *node) {
  PNULL_CHECK_DEFAULT(this);
  PNULL_CHECK_DEFAULT(node);

  if (this->head == -1) {
    this->head = 0;
    this->tail = 0;
    this->list[this->head] = node;
  } else {
    ssize_t tail_idx = this->tail;
    ssize_t newtail_idx = ++this->tail;
    ArrayListNode *tail_obj = this->list[tail_idx];
    ArrayListNode **newtail_obj = &this->list[newtail_idx];

    *newtail_obj = node;
    tail_obj->next = newtail_idx;
    (*newtail_obj)->prev = tail_idx;
  }
}