#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"
#include "uarray2b.h"
#include "uarray2.h"

typedef A2Methods_UArray2 A2;

/* 
 * passed to the apply functions to give access to the method and the new
 * image map where the transformed image will be stored
 */
typedef struct Closure {
    A2 rotatedImage;
    //A2Methods_T methods;
    //int height, width;
} Closure;

static int height;
static int width;
static A2Methods_T methods;

enum Methods { block, row, col};

void rotate90Blocked(int col, int row, UArray2b_T array2, void* ptr, void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;

    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, height - row - 1,
        col);

    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate180Blocked(int col, int row, UArray2b_T array2, void* ptr, 
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*) cl;
    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, width - col - 1,
        height - row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate270Blocked(int col, int row, UArray2b_T array2, void* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, row, width - 
        col - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void flipHorizontalBlocked(int col, int row, UArray2b_T array2, void* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, width - 
        col - 1, row);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;    
}

void flipVerticalBlocked(int col, int row, UArray2b_T array2, void* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, col, height -
        row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void transposeImageBlocked(int col, int row, UArray2b_T array2, void* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2b_at(
        closure->rotatedImage, width - col -1 ,
        height - row -1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}
void rotate90(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*)cl;

    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, height - row - 1,
        col);

    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate180(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*) cl;
    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, width - col - 1,
        height - row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate270(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, row, width - 
        col - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void flipHorizontal(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, width - 
        col - 1, row);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;    
}

void flipVertical(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, col, height -
        row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void transposeImage(int col, int row, void* ptr, void* cl) {
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = UArray2_at(
        closure->rotatedImage, width - col -1 ,
        height - row -1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

int main(int argc, char *argv[]) {
  int rotation = 0;
  int flip = 0; // 1 for horizontal, 2 for vertical
  int transpose = 0;
  methods = uarray2_methods_plain; // default to UArray2 methods
  assert(methods);
  enum Methods methods_type = row;
  A2Methods_mapfun *map = methods->map_default; // default to best map
  assert(map);
#define SET_METHODS(METHODS, MAP, WHAT) do { \
      methods = (METHODS); \
      assert(methods); \
      map = methods->MAP; \
      if (!map) { \
        fprintf(stderr, "%s does not support " WHAT "mapping\n", argv[0]); \
        exit(1); \
      } \
    } while(0)

  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-row-major")) {
      SET_METHODS(uarray2_methods_plain, map_row_major, "row-major");
      methods_type = row;
    } else if (!strcmp(argv[i], "-col-major")) {
      SET_METHODS(uarray2_methods_plain, map_col_major, "column-major");
      methods_type = col;
    } else if (!strcmp(argv[i], "-block-major")) {
      SET_METHODS(uarray2_methods_blocked, map_block_major, "block-major");
      methods_type = block;
    } else if (!strcmp(argv[i], "-rotate")) {
      assert(i + 1 < argc);
      char *endptr;
      rotation = strtol(argv[++i], &endptr, 10);
      assert(*endptr == '\0'); // parsed all correctly
      assert(rotation == 0   || rotation == 90
          || rotation == 180 || rotation == 270);
    } else if (!strcmp(argv[i], "-flip")) {
      assert(i + 1 < argc);
      if(!strcmp(argv[i+1], "horizontal")) {
          flip = 1;
      }
      else if(!strcmp(argv[i+1], "vertical")) {
          flip = 2;
      }
      i++;
      assert(flip == 1 || flip == 2);
    } else if (!strcmp(argv[i], "-transpose")) {
      transpose = 1;
    } else if (*argv[i] == '-') {
      fprintf(stderr, "%s: unknown option '%s'\n", argv[0], argv[i]);
      exit(1);
    } else if (argc - i > 2) {
      fprintf(stderr, "Usage: %s [-rotate <angle>] "
              "[-{row,col,block}-major] [filename]\n", argv[0]);
      exit(1);
    } else {
    break;
    }
  }

    Pnm_ppm unrotated;
    if (i < argc) {
        FILE* fp = fopen(argv[i], "r");
        if (fp == NULL) {
            perror(argv[i]);
            exit(1);
        }
        unrotated = Pnm_ppmread(fp, methods);
        fclose(fp);
    } 
    else {
        unrotated = Pnm_ppmread(stdin, methods);
    }   

   int rotatedWidth = 0;
   int rotatedHeight = 0;
   int rotatedDenominator = unrotated->denominator;

   if(rotation == 90 || rotation == 270) {
        rotatedWidth = unrotated->height;
        rotatedHeight = unrotated->width;
   }
   else if (rotation == 180 || flip > 0 || rotation == 0 || transpose == 1){
        rotatedWidth = unrotated->width;
        rotatedHeight = unrotated->height;
   }
   
   /* blank image map with correct dimensions to store transformed image */
   A2 rotated = unrotated->methods->new(rotatedWidth, rotatedHeight, 
        sizeof(struct Pnm_rgb));
   Closure cl;
   cl.rotatedImage = rotated;
   methods = methods;
   height = unrotated->height;
   width = unrotated->width;

    if(methods_type == col) {
        if(rotation == 90) {
            UArray2_map_col_major(unrotated->pixels, rotate90, &cl);
        }
        else if (rotation == 180) {
            UArray2_map_col_major(unrotated->pixels, rotate180, &cl);
        }
        else if (rotation == 270) {
            UArray2_map_col_major(unrotated->pixels, rotate270, &cl);
        }
        else if (flip == 1) {
            UArray2_map_col_major(unrotated->pixels, flipHorizontal, &cl);
        }
        else if (flip == 2) {
            UArray2_map_col_major(unrotated->pixels, flipVertical, &cl);
        }
        else if (transpose == 1) {
            UArray2_map_col_major(unrotated->pixels, transposeImage, &cl);
        }
        else if (rotation == 0) {
            Pnm_ppmwrite(stdout, unrotated);
            Pnm_ppmfree(&unrotated);
            methods->free(&rotated);
            return 0;
        }
    }

    if(methods_type == block) {
        if(rotation == 90) {
            UArray2b_map(unrotated->pixels, rotate90Blocked, &cl);
        }
        else if (rotation == 180) {
            UArray2b_map(unrotated->pixels, rotate180Blocked, &cl);
        }
        else if (rotation == 270) {
            UArray2b_map(unrotated->pixels, rotate270Blocked, &cl);
        }
        else if (flip == 1) {
            UArray2b_map(unrotated->pixels, flipHorizontalBlocked, &cl);
        }
        else if (flip == 2) {
            UArray2b_map(unrotated->pixels, flipVerticalBlocked, &cl);
        }
        else if (transpose == 1) {
            UArray2b_map(unrotated->pixels, transposeImageBlocked, &cl);
        }
        else if (rotation == 0) {
            Pnm_ppmwrite(stdout, unrotated);
            Pnm_ppmfree(&unrotated);
            methods->free(&rotated);
            return 0;
        }
    }

    if(methods_type == row) {
        if(rotation == 90) {
            UArray2_map_row_major(unrotated->pixels, rotate90, &cl);
        }
        else if (rotation == 180) {
            UArray2_map_row_major(unrotated->pixels, rotate180, &cl);
        }
        else if (rotation == 270) {
            UArray2_map_row_major(unrotated->pixels, rotate270, &cl);
        }
        else if (flip == 1) {
            UArray2_map_row_major(unrotated->pixels, flipHorizontal, &cl);
        }
        else if (flip == 2) {
            UArray2_map_row_major(unrotated->pixels, flipVertical, &cl);
        }
        else if (transpose == 1) {
            UArray2_map_row_major(unrotated->pixels, transposeImage, &cl);
        }
        else if (rotation == 0) {
            Pnm_ppmwrite(stdout, unrotated);
            Pnm_ppmfree(&unrotated);
            methods->free(&rotated);
            return 0;
        }
    }

   Pnm_ppmfree(&unrotated);

   Pnm_ppm rotatedImage;
   NEW(rotatedImage);
   rotatedImage->width = rotatedWidth;
   rotatedImage->height = rotatedHeight;
   rotatedImage->denominator = rotatedDenominator;
   rotatedImage->pixels = rotated;
   rotatedImage->methods = methods;

   Pnm_ppmwrite(stdout, rotatedImage);

   Pnm_ppmfree(&rotatedImage);
}
