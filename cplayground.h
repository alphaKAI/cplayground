#ifndef __CPLAYGROUND_HEADER_INCLUDED__
#define __CPLAYGROUND_HEADER_INCLUDED__

#include "sds/sds.h"
#include <stdbool.h>
#include <stddef.h>

///////////////// util ////////////////
void *xmalloc(size_t);
void xfree(void **);

#define xnew(T) (xmalloc(sizeof(T)))

/////////////// binary tree ///////////////
#include "binarytree.h"

///////////////   Vector   ///////////////
#include "vector.h"

///////////////   QUEUE    ///////////////
#include "queue.h"

/////////////// FUNCTIONAL ///////////////
#include "functional.h"

///////////////     AVL    ///////////////
#include "avl.h"

//////////////      Map    ///////////////
#include "map.h"

#endif
