#include <memory/heap.h>
#include <memory/pageframe.h>
#include <utils/logger.h>

Heap Heap::k;

void Heap::init() {
#if KDEBUG == true
    Logger::info("Initializing kernel heap...");
#endif
    first = last = (HeapBlockHeader *)PageFrameAllocator::alloc();
    first->size = 4096;
    first->previous = nullptr;
    first->next = nullptr;
    first->used = false;
#if KDEBUG == true
    Logger::ok();
#endif
}

void Heap::free(void *address) {
    HeapBlockHeader *cur = first;
    while (cur) {
        if ((unsigned long long)address >= (unsigned long long)cur &&
        (unsigned long long)address < (unsigned long long)cur + cur->size) {
            cur->used = false;
            if ((unsigned long long)cur->next == 
            (unsigned long long)cur + cur->size 
            && !cur->next->used) {
                merge_blocks(cur, cur->next);
            }
            if ((unsigned long long)cur == 
            (unsigned long long)cur->previous + cur->previous->size 
            && !cur->previous->used) {
                merge_blocks(cur->previous, cur);
            }
        }
        cur = cur->next;
    }
}

void *Heap::alloc(unsigned long long size) {
    HeapBlockHeader *cur = first;
    while (cur) {
        if (cur->size > size + sizeof(HeapBlockHeader) && !cur->used) {
            if (cur->size > size + 2*sizeof(HeapBlockHeader) + 1) {
                split_block(cur, size + sizeof(HeapBlockHeader));
            }
            cur->used = true;
            return (void *)((unsigned long long)cur + sizeof(HeapBlockHeader));
        }
        cur = cur->next;
    }

    expand((size + sizeof(HeapBlockHeader)) / 4096 + 1);
    return alloc(size);
}

void Heap::expand(unsigned long long length) {
    HeapBlockHeader *tmp_last = last;
    last = (HeapBlockHeader *)PageFrameAllocator::alloc(length);
    last->size = length * 4096;
    last->previous = tmp_last;
    last->next = nullptr;
    last->used = false;
    tmp_last->next = last;

    if ((unsigned long long)last == 
    (unsigned long long)tmp_last + tmp_last->size 
    && !tmp_last->used) {
        merge_blocks(tmp_last, last);
    }
}

HeapBlockHeader *Heap::split_block(HeapBlockHeader *block, unsigned long long first_size) {
    HeapBlockHeader *second = (HeapBlockHeader *)((unsigned long long)block + first_size);
    second->size = block->size - first_size;
    second->previous = block;
    second->next = block->next;
    second->used = block->used;
    second->next->previous = second;
    block->size = first_size;
    block->next = second;
    if (last == block) last = second;
    return second;
}

HeapBlockHeader *Heap::merge_blocks(HeapBlockHeader *first, HeapBlockHeader *last) {
    first->size = (unsigned long long)last + last->size - (unsigned long long)first;
    first->next = last->next;
    first->next->previous = first;
    if (this->last == last) this->last = first;
    return first;
}

void *operator new(unsigned long int size)
{
    return Heap::k.alloc(size);
}
 
void *operator new[](unsigned long int size)
{
    return Heap::k.alloc(size);
}
 
void operator delete(void *p)
{
    Heap::k.free(p);
}
 
void operator delete[](void *p)
{
    Heap::k.free(p);
}