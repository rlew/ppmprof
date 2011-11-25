#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// elide start
#include <errno.h>
// elide stop

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
// elide start
#include "arotate.h"
// elide stop

int main(int argc, char *argv[]) {
  int rotation = 0;
  A2Methods_T methods = uarray2_methods_plain; // default to UArray2 methods
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
  // ...
// elide start
  Pnm_ppm ppm; // original pixmap as read from the input
  if (i < argc) {
    FILE* fp = fopen(argv[i], "r");
    if (fp == NULL) {
      perror(argv[i]);
      exit(1);
    }
    ppm = Pnm_ppmread(fp, methods);
    fclose(fp);
  } else {
    ppm = Pnm_ppmread(stdin, methods);
  }
  A2Methods_UArray2 orig = ppm->pixels; // image before rotation
  A2Methods_UArray2 rot  = NULL;        // image after rotation
  switch(rotation) {
    case 0:   rot = orig; break;
    case 90:  rot = rotate_cw (methods, orig, map); break;
    case 180: rot = rotate_180(methods, orig, map); break;
    case 270: rot = rotate_ccw(methods, orig, map); break;
    default:  assert(0); break;
  }
  assert(rot);
  if (rot != orig) { // set ppm to hold the new image
    assert(orig == ppm->pixels);
    methods->free(&ppm->pixels);
    ppm->pixels = rot;
    ppm->width  = methods->width(rot);
    ppm->height = methods->height(rot);
  }
  Pnm_ppmwrite(stdout, ppm);
  Pnm_ppmfree(&ppm);
  return 0;
// elide stop
}
