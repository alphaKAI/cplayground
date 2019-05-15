#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sds/sds.h"
#include "cplayground.h"

BinaryTree* new_BinaryTree(void* val) {
  BinaryTree* bt = xmalloc(sizeof(BinaryTree));
  bt->val = val;
  bt->left = NULL;
  bt->right = NULL;
  return bt;
}

void free_BinaryTree(BinaryTree* root, BT_VAL_FREE free_val) {
  if (root == NULL) {
    return;
  }
  free_BinaryTree(root->left, free_val);
  root->left = NULL;
  
  free_BinaryTree(root->right, free_val);
  root->right = NULL;
  
  free_val(root->val);
  root->val = NULL;
  free(root);
}

BinaryTree* insert(BinaryTree* root, void* val, BT_VAL_CMP cmp) {
  if (root == NULL) {
    return new_BinaryTree(val);
  }

  // root->val >= val
  if (cmp(root->val, val) <= 0) {
    root->left = insert(root->left, val, cmp);
  }

  // root->val < val
  if (cmp(root->val, val) > 0) {
    root->right = insert(root->right, val, cmp);
  }
 
  return root;
}

BinaryTree* find(BinaryTree* root, void* val, BT_VAL_CMP cmp) {
  if (root == NULL) {
    return NULL;
  }

  if (cmp(root->val, val) == 0) {
    return root;
  }

  return find(val < root->val ? root->left : root->right, val, cmp);
}

bool exsists(BinaryTree* root, void* val, BT_VAL_CMP cmp) {
  return find(root, val, cmp) != NULL;
}

static sds rep_str(sds pat, size_t n) {
  sds ret = sdsempty();

  for (size_t i = 0; i < n; i++) {
    ret = sdscatprintf(ret, "%s", pat);
  }

  return ret;
}

static sds stringifyImpl(BinaryTree* root, BT_VAL_SHOW show, size_t depth);

static sds stringifyORNull(BinaryTree* t, BT_VAL_SHOW show, size_t depth) {
  if (t == NULL) {
    return sdsnew("NULL");
  }

  return stringifyImpl(t, show, depth);
}

static sds stringifyImpl(BinaryTree* root, BT_VAL_SHOW show, size_t depth) {
  sds pad = rep_str("  ", depth);

  return sdscatprintf(sdsempty(),
"%s<%s>"
"\n%s |L--%s"
"\n%s |R--%s", pad, show(root->val), pad, stringifyORNull(root->left, show, depth + 1),
                                                    pad, stringifyORNull(root->right, show, depth + 1));
}

sds stringify(BinaryTree* root, BT_VAL_SHOW show) {
  return stringifyImpl(root, show, 0);
}
