#pragma once

#include <init/boot.h>
#include <memory/memory.h>
#include <utils/bitmap.h>
#include <utils/utils.h>

struct HeapBlockHeader;
typedef struct HeapBlockHeader HeapBlockHeader;

struct HeapBlockHeader{
    unsigned long long size;
    HeapBlockHeader *previous;
    HeapBlockHeader *next;
    bool used;
};

class Heap {
public:
    void init();
    void free(void *address);
    void *alloc(unsigned long long size);

    static Heap k;

private:
    HeapBlockHeader *first;
    HeapBlockHeader *last;

    void expand(unsigned long long length);
    HeapBlockHeader *split_block(HeapBlockHeader *block, unsigned long long first_size);
    HeapBlockHeader *merge_blocks(HeapBlockHeader *first, HeapBlockHeader *last);
};

void *operator new(unsigned long int size);
void *operator new[](unsigned long int size);
void operator delete(void *p);
void operator delete[](void *p);
