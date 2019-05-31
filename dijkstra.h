#ifndef __DIJKSTRA_HEADER_INCLUDED__
#define __DIJKSTRA_HEADER_INCLUDED__
#include "cplayground.h"

extern const long INF;

typedef struct {
  long dst;
  long cost;
} Edge;

Edge *new_Edge(long dst, long cost);

Vector *dijkstra(Vector *tree, long src);
#endif