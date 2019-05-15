#include "cplayground.h"
#include "sds/sds.h"
#include <string.h>

Map *new_map(void) {
  Map *map = xmalloc(sizeof(Map));
  map->tree = new_AVLTree();
  return map;
}

static int map_compare(sds a, sds b) {
  int ret = strcmp(a, b);
  if (ret < 0) {
    return -1;
  } else if (ret == 0) {
    return 0;
  } else {
    return 1;
  }
}

void map_put(Map *map, sds key, void *val) {
  avl_insert(map->tree, key, val, (ELEM_COMPARE)&map_compare);
}

void map_puti(Map *map, sds key, int val) {
  map_put(map, key, (void *)(intptr_t)val);
}

void *map_get(Map *map, sds key) {
  return avl_find(map->tree, key, (ELEM_COMPARE)&map_compare);
}
