#ifndef __MAP_HEADER_INCLUDED__
#define __MAP_HEADER_INCLUDED__
#include "cplayground.h"

typedef struct {
  AVLTree *tree;
} Map;

Map *new_map(ELEM_COMPARE compare);
void map_put(Map *map, void *key, void *val);
void map_puti(Map *map, void *key, int val);
void *map_get(Map *map, void *key);
bool map_exists(Map *map, void *key);
#endif