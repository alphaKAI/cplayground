#ifndef __BINARYTREE_HEADER_INCLUDED__
#define __BINARYTREE_HEADER_INCLUDED__

#include "sds/sds.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct BinaryTree {
  void *val;
  struct BinaryTree *left;
  struct BinaryTree *right;
} BinaryTree;

typedef void (*BT_VAL_FREE)(void *);
typedef int (*BT_VAL_CMP)(void *, void *);
typedef sds (*BT_VAL_SHOW)(void *);

BinaryTree *new_BinaryTree(void *val);
void free_BinaryTree(BinaryTree *root, BT_VAL_FREE free_val);
BinaryTree *insert(BinaryTree *root, void *val, BT_VAL_CMP cmp);
BinaryTree *find(BinaryTree *root, void *val, BT_VAL_CMP cmp);
bool exists(BinaryTree *root, void *val, BT_VAL_CMP cmp);

sds stringify(BinaryTree *root, BT_VAL_SHOW show);

#endif