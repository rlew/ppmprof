#ifndef MEMSEG_INCLUDED
#define MEMSEG_INCLUDED
#include <stdlib.h>
#include <stdint.h>
#include "seq.h"
#include "uarray.h"

typedef uint32_t UM_Word;
typedef struct Mem Mem;

Mem* newMem();
void loadProgram(Mem* memorySegments, UM_Word ID);
void mapInstructions(int length);
void loadInstruction(int offest, UM_Word value);
UM_Word getInstruction(int offset);//, UM_Word offset);
int instructionLength();
void instantiateMem(Mem* memorySegments, int length);
UM_Word mapSegment(Mem* memorySegments, int length);
void unmapSegment(Mem* memorySegments, UM_Word index);
UM_Word segmentedLoad(Mem* memorySegments, UM_Word ID, int offset);
void segmentedStore(Mem* memorySegments, UM_Word ID, int offset, UM_Word
                       value);
void freeMem(Mem* memorySegments);
#endif
