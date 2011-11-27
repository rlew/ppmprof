#include <assert.h>
#include <string.h>

#include "arotate.h"

typedef A2Methods_UArray2 A2;
typedef A2Methods_mapfun Mapfun;

struct cl {
	unsigned w, h; // height and width of *original* array
	A2 array;      // elements of *new* array
	unsigned size; // number of bytes in one element;
	A2Methods_Object *(*at)(A2 array2, int i, int j);
};
#line 62 "www/solutions/arotate.nw"
static void move_element_cw(int i, int j, A2 old, void *elem, void *cl) {
	(void)old;
	struct cl *new = cl;
	memcpy(new->at(new->array, new->h - j - 1, i), elem, new->size);
}

static void move_element_ccw(int i, int j, A2 old, void *elem, void *cl) {
	(void)old;
	struct cl *new = cl;
	memcpy(new->at(new->array, j, new->w - i - 1), elem, new->size);
}

static void move_element_180(int i, int j, A2 old, void *elem, void *cl) {
	(void)old;
	struct cl *new = cl;
	memcpy(new->at(new->array, new->w - i - 1, new->h - j - 1), elem, new->size);
}
#line 83 "www/solutions/arotate.nw"
static A2 rotate(A2Methods_T m, A2Methods_applyfun app, Mapfun map, A2 a) {
	assert(m && app && map && a);
	int w = m->width(a);
	int h = m->height(a);
	int size = m->size(a);
	int b = m->blocksize(a);
	A2 new = (app == move_element_180)
	? m->new_with_blocksize(w, h, size, b)
	: m->new_with_blocksize(h, w, size, b);
	struct cl info = { w, h, new, size, m->at };
	map(a, app, &info);
	return new;
}
#line 100 "www/solutions/arotate.nw"
A2 rotate_cw(A2Methods_T methods, A2 a, Mapfun map) {
	return rotate(methods, move_element_cw, map, a);
}
A2 rotate_180(A2Methods_T methods, A2 a, Mapfun map) {
	return rotate(methods, move_element_180, map, a);
}
A2 rotate_ccw(A2Methods_T methods, A2 a, Mapfun map) {
	return rotate(methods, move_element_ccw, map, a);
}
