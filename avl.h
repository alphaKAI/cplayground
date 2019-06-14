#ifndef __AVL_HEADER_INCLUDED__
#define __AVL_HEADER_INCLUDED__

#include <stdbool.h>
#include <stddef.h>

///////////////////// AVL /////////////////////

typedef struct AVLNode_t {
  void *key;
  void *value;
  int height;
  int size;
  struct AVLNode_t *left;
  struct AVLNode_t *right;
} AVLNode;

AVLNode *new_AVLNode(void *key, void *value);

typedef struct {
  AVLNode *root;
  ELEM_COMPARE compare;
} AVLTree;

AVLTree *new_AVLTree(ELEM_COMPARE compare);

typedef void (*ELEM_FREE)(void *);

void free_AVLNode(AVLNode **n_ptr, ELEM_FREE free_key, ELEM_FREE free_val);
void free_AVLTree(AVLTree **t_ptr, ELEM_FREE free_key, ELEM_FREE free_val);

typedef int (*ELEM_COMPARE)(void *, void *);

bool avl_exists(AVLTree *tree, void *key);

void *avl_find(AVLTree *tree, void *key);

void avl_insert(AVLTree *tree, void *key, void *value);
void avl_delete(AVLTree *tree, void *key);

#define sz(t) (t ? t->size : 0)
#define ht(t) (t ? t->height : 0)

enum { L, R };

void avl_print_node(AVLNode *node, size_t depth, ELEM_PRINTER key_printer,
                    ELEM_PRINTER value_printer);

void avl_print_tree(AVLTree *tree, ELEM_PRINTER key_printer,
                    ELEM_PRINTER value_printer);

Vector *avl_values(AVLTree *tree);
Vector *avl_keys(AVLTree *tree);

#endif
