#ifndef __MAP_HEADER_INCLUDED__
#define __MAP_HEADER_INCLUDED__
#include "cplayground.h"

typedef struct {
  AVLTree *tree;
} Map;

Map *new_map(void);
void map_put(Map *map, sds key, void *val);
void map_puti(Map *map, sds key, int val);
void *map_get(Map *map, sds key);
bool map_exists(Map *map, sds key);
#endif