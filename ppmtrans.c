#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"

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

void rotate90(int col, int row, A2 array2, A2Methods_Object* ptr, 
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;

    struct Pnm_rgb* rotatedElem = methods->at(
        closure->rotatedImage, height - row - 1,
        col);

    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate180(int col, int row, A2 array2, A2Methods_Object* ptr, 
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*) cl;
    struct Pnm_rgb* rotatedElem = methods->at(
        closure->rotatedImage, width - col - 1,
        height - row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void rotate270(int col, int row, A2 array2, A2Methods_Object* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = methods->at(
        closure->rotatedImage, row, width - 
        col - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void flipHorizontal(int col, int row, A2 array2, A2Methods_Object* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = methods->at(
        closure->rotatedImage, width - 
        col - 1, row);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;    
}

void flipVertical(int col, int row, A2 array2, A2Methods_Object* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = methods->at(
        closure->rotatedImage, col, height -
        row - 1);
    *(struct Pnm_rgb*)rotatedElem = *(struct Pnm_rgb*)ptr;
}

void transposeImage(int col, int row, A2 array2, A2Methods_Object* ptr,
    void* cl) {
    (void) array2;
    Closure* closure = (Closure*)cl;
    struct Pnm_rgb* rotatedElem = methods->at(
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
    } else if (!strcmp(argv[i], "-col-major")) {
      SET_METHODS(uarray2_methods_plain, map_col_major, "column-major");
    } else if (!strcmp(argv[i], "-block-major")) {
      SET_METHODS(uarray2_methods_blocked, map_block_major, "block-major");
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

   if(rotation == 90) {
       map(unrotated->pixels, rotate90, &cl);
   }
   else if (rotation == 180) {
       map(unrotated->pixels, rotate180, &cl);
   }
   else if (rotation == 270) {
       map(unrotated->pixels, rotate270, &cl);
   }
   else if (flip == 1) {
       map(unrotated->pixels, flipHorizontal, &cl);
   }
   else if (flip == 2) {
       map(unrotated->pixels, flipVertical, &cl);
   }
   else if (transpose == 1) {
       map(unrotated->pixels, transposeImage, &cl);
   }
   else if (rotation == 0) {
       Pnm_ppmwrite(stdout, unrotated);
       Pnm_ppmfree(&unrotated);
       methods->free(&rotated);
       return 0;
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
