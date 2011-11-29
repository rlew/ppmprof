#include <math.h>
#include "assert.h"
#include "mem.h"
#include "uarray.h"
#include "uarray2.h"
#include "uarray2b.h"
#include <stdlib.h>

#define T UArray2b_T

struct T { // represents a 2D array of cells each of size 'size'
  int width, height;
  unsigned blocksize;
  unsigned size;
  void* blocks;
    /* matrix of blocks, each blocksize * blocksize */
    /* matrix dimensions are width and height divided by blocksize,
       rounded up */
    /* a block is an Array_T of length blocksize * blocksize and size
       'size' */
    /* invariant relating cells in blocks to cells in the abstraction
       described in section on coordinate transformations below */
};
#include <stdio.h>  /* include so we can print diagnostics */

T UArray2b_new(int width, int height, int size, int blocksize) {
  assert(blocksize > 0);
  T array;
  NEW(array);
  array->width  = width;
  array->height = height;
  array->size   = size;
  array->blocksize = blocksize;
  int xblocks = (width + blocksize - 1) / blocksize;
  int yblocks = (height + blocksize - 1) / blocksize;
  array->blocks = malloc(size * xblocks * blocksize * yblocks * 
                         blocksize);
  return array;
}

void UArray2b_free(T *array2b) {
  assert(array2b && *array2b);
  T array = *array2b;
  free(array->blocks);
  FREE(*array2b);
}

T UArray2b_new_64K_block(int width, int height, int size) {
  int blocksize = (int) floor(sqrt((double) (64 * 1024) / (double) size));
  if (blocksize == 0)
    blocksize = 1;
  // as big as possible
  assert((blocksize+1) * (blocksize+1) * size > 64 * 1024);
  if (size <= 64 * 1024)
    assert(blocksize * blocksize * size <= 64 * 1024); // no bigger
  return UArray2b_new(width, height, size, blocksize);
}

void *UArray2b_at(T array2b, int i, int j) {
  assert(i >= 0 && j >= 0);
  int b  = array2b->blocksize;
  int loc = ((j / b)* ((array2b->width + b - 1) / b) + (i / b))*(b*b) + (i % b) * b + j % b;
  return &((unsigned char*)array2b->blocks)[loc * array2b->size];
}
void UArray2b_map(T array2b, 
    void apply(int i, int j, T array2b, void *elem, void *cl), void *cl) {
  assert(array2b);
  int h = array2b->height;
  int w = array2b->width;
  int b = array2b->blocksize;
  void* blocks = array2b->blocks;
  int size = array2b->size;
  int bw = (w + b - 1) / b;
  int bh = (h + b - 1) / b;

  for (int bx = 0; bx < bw; bx++) {
      for (int by = 0; by < bh; by++) {
            int bi = bx * b;
          for(int i0 = 0; i0 < b; i0++) {
                int bj = by * b;
              for(int j0 = 0; j0 < b; j0++) {
                  if(bi < w && bj < h){  
                      int loc = (by * bw + bx)*(b*b) + 
                                (bi % b) * b + bj % b;
                      apply(bi, bj, array2b, &((unsigned
                      char*)blocks)[loc*size], cl);
                  }
                  bj++;
              }
              bi++;
           }
      }
   }
}

int UArray2b_height(T array2b) {
  assert(array2b);
  return array2b->height;
}
int UArray2b_width(T array2b) {
  assert(array2b);
  return array2b->width;
}
int UArray2b_size(T array2b) {
  assert(array2b);
  return array2b->size;
}
int UArray2b_blocksize(T array2b) {
  assert(array2b);
  return array2b->blocksize;
}

int UArray2b_version_uses_UArray2_T = 1;
