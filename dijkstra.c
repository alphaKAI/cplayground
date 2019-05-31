#include "cplayground.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const long INF = 1 << 30;

Edge *new_Edge(long dst, long cost) {
  Edge *edge = xmalloc(sizeof(Edge));
  edge->dst = dst;
  edge->cost = cost;
  return edge;
}

static int cmp_Edge(void *lhs, void *rhs) {
  Edge *le = (Edge *)lhs;
  Edge *re = (Edge *)rhs;

  if (le->cost >= re->cost) {
    return 1;
  } else {
    return -1;
  }
}

Vector *dijkstra(Vector *tree, long src) {
  size_t V = tree->len;
  Vector *dist = new_vec_with(V);
  for (size_t i = 0; i < V; i++) {
    dist->data[i] = INT_TO_VoPTR(INF);
  }

  PriorityQueue *pqueue = new_PriorityQueue(cmp_Edge);

  dist->data[src] = 0;
  pqueue_push(pqueue, new_Edge(src, 0));

  while (!pqueue_empty(pqueue)) {
    Edge *e = pqueue_pop(pqueue);
    long v = e->dst, cost = e->cost;

    if (VoPTR_TO_INT(dist->data[v]) < cost) {
      continue;
    }

    Vector *vec = tree->data[v];
    VecForeachWithType(vec, Edge *, e, {
      if (VoPTR_TO_INT(dist->data[v]) + e->cost <
          VoPTR_TO_INT(dist->data[e->dst])) {
        dist->data[e->dst] =
            INT_TO_VoPTR(VoPTR_TO_INT(dist->data[v]) + e->cost);
        pqueue_push(pqueue, new_Edge(e->dst, VoPTR_TO_INT(dist->data[e->dst])));
      }
    });
  }

  return dist;
}