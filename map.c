#include "cplayground.h"
#include "sds/sds.h"
#include <string.h>

Map *new_map(ELEM_COMPARE compare) {
  Map *map = xmalloc(sizeof(Map));
  map->tree = new_AVLTree(compare);
  return map;
}

void map_put(Map *map, void *key, void *val) {
  avl_insert(map->tree, key, val);
}

void map_puti(Map *map, void *key, int val) {
  map_put(map, key, (void *)(intptr_t)val);
}

void *map_get(Map *map, void *key) { return avl_find(map->tree, key); }
