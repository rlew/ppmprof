#include "uarray.h"
#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "assert.h"

#define T UArray2_T

struct T{
    int width;
    int height;
    int size;
    void* array;
};

int UArray2_height(T twoDArray) {
	assert(twoDArray);
	return twoDArray->height;
}

int UArray2_width(T twoDArray) {
    assert(twoDArray);
	return twoDArray->width;
}

int UArray2_size(T twoDArray) {
	assert(twoDArray);
    return twoDArray->size;
}

T UArray2_new(int width, int height, int size) {
	assert(size > 0 && height > 0 && width > 0);
    T twoDArray = malloc(sizeof(*twoDArray));
    int length = height * width;
    twoDArray->array = malloc(size*length);
    twoDArray->height = height;
    twoDArray->width = width;
    twoDArray->size = size;
	assert(twoDArray);
    return twoDArray;
}

void UArray2_free(T* twoDArray) {
    assert(*twoDArray);
	free((*twoDArray)->array);
    free(*twoDArray);
}

void* UArray2_at(T twoDArray, int col, int row) {
    assert(twoDArray);
	assert(row < twoDArray->height && col < twoDArray->width);
	assert(row >= 0 && col >= 0);
	int loc = row * UArray2_width(twoDArray) + col;
    return &((unsigned char*)twoDArray->array)[loc*twoDArray->size]; 
}

void UArray2_map_row_major(T twoDArray, void apply(int col, int row, void* elem, 
    void* cl), void* cl){
	assert(twoDArray);
    for(int r=0; r < UArray2_height(twoDArray); r++) {
        for(int c=0; c < UArray2_width(twoDArray); c++) {
            apply(r, c, UArray2_at(twoDArray, c, r), cl);
        }
    }
}

void UArray2_map_col_major(T twoDArray, void apply(int col, int row, void* elem, 
    void* cl), void* cl) {
	assert(twoDArray);
    for(int c=0; c < UArray2_width(twoDArray); c++){
        for(int r=0; r < UArray2_height(twoDArray); r++){
            apply(r, c, UArray2_at(twoDArray, c, r), cl);
        }
    }
}

#undef T
