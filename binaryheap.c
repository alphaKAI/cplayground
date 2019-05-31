#include "cplayground.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define GetHeapVec(heap) (heap->vec)
#define GetHeapData(heap, i) (heap->vec->data[i])

BinaryHeap *new_BinaryHeap(ELEM_COMPARE compare) {
  BinaryHeap *heap = xmalloc(sizeof(BinaryHeap));
  heap->vec = new_vec();
  heap->compare = compare;
  return heap;
}

void heap_insert(BinaryHeap *heap, void *val) {
  size_t n = GetHeapVec(heap)->len;
  vec_push(GetHeapVec(heap), val);

  while (n != 0) {
    size_t i = (n - 1) / 2;

    // heap->vec->data[n] > heap->vec->data[i]
    if (heap->compare(GetHeapData(heap, n), GetHeapData(heap, i)) == 1) {
      void *tmp = GetHeapData(heap, n);
      GetHeapData(heap, n) = GetHeapData(heap, i);
      GetHeapData(heap, i) = tmp;
    }

    n = i;
  }
}

void *heap_pop(BinaryHeap *heap) {
  assert(GetHeapVec(heap)->len > 0);

  size_t n = GetHeapVec(heap)->len - 1;
  void *ret = GetHeapData(heap, 0);
  GetHeapData(heap, 0) = GetHeapData(heap, n);
  vec_pop(GetHeapVec(heap));

  for (size_t i = 0, j; (j = 2 * i + 1) < n;) {
    if ((j != n - 1) &&
        heap->compare(GetHeapData(heap, j), GetHeapData(heap, j + 1)) == -1) {
      j++;
    }

    if (heap->compare(GetHeapData(heap, i), GetHeapData(heap, j)) == -1) {
      void *tmp = GetHeapData(heap, j);
      GetHeapData(heap, j) = GetHeapData(heap, i);
      GetHeapData(heap, i) = tmp;
    }

    i = j;
  }

  return ret;
}

bool heap_empty(BinaryHeap *heap) { return GetHeapVec(heap)->len == 0; }

void print_heap(BinaryHeap *heap, ELEM_PRINTER show) {
  size_t n = GetHeapVec(heap)->len;
  size_t i = 0;

  while (i < (n / 2)) {
    size_t l_idx = i * 2 + 1;
    size_t r_idx = i * 2 + 2;

    printf("<%s, %s, %s>\n", show(GetHeapData(heap, i)),
           show(GetHeapData(heap, l_idx)), show(GetHeapData(heap, r_idx)));

    i++;
  }
}