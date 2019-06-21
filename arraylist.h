#ifndef __ARRAYLIST_HEADER_INCLUDED__
#define __ARRAYLIST_HEADER_INCLUDED__

typedef struct {
  ssize_t next, prev;
} ArrayListNode;

typedef struct {
  ArrayListNode **list;
  ssize_t list_size;
  ssize_t head, tail;
} ArrayList;

#define print_ArrayList(ElemType, list, member, visalize)                      \
  {                                                                            \
    ElemType *elem;                                                            \
    char *s;                                                                   \
    for (ssize_t idx = list->head; idx != -1 && idx <= list->tail;             \
         idx = list->list[idx]->next) {                                        \
      elem = container_of(list->list[idx], ElemType, member);                  \
      s = visalize(elem);                                                      \
      printf("list[%ld] %s\n", idx, s);                                        \
      free(s);                                                                 \
    }                                                                          \
  }

#define init_ArrayListNode(node)                                               \
  {                                                                            \
    PNULL_CHECK_DEFAULT((node));                                               \
    (node)->next = -1;                                                         \
    (node)->prev = -1;                                                         \
  }

ArrayListNode *new_ArrayListNode(void);
ArrayList *new_ArrayList(ssize_t list_size);
void append_ArrayList(ArrayList *this, ArrayListNode *node);

#endif