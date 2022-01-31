#include <memory/pageframe.h>

#include <drivers/serial.h>
#include <utils/utils.h>
#include <utils/logger.h>

Bitmap PageFrameAllocator::pageFrames;
unsigned long long PageFrameAllocator::freeMemory;
unsigned long long PageFrameAllocator::usedMemory;

extern unsigned long long _kernelStart;
extern unsigned long long _kernelEnd;
extern char buffer[256];

void PageFrameAllocator::init() {
    LOG_DEBUG("Initializing page frame allocator...");

    // Find the largest free hole in memory
    void *largestFreeSegment = nullptr;
    unsigned long long largestFreeSegmentSize = 0;
    for (int i = 0; i < bInfo.memmap.descCount; ++i) {
        EFIMemoryDescriptor *desc = (EFIMemoryDescriptor *)((unsigned long long)bInfo.memmap.base_addr + i * bInfo.memmap.descSize);
        if (desc->type == 7) { // EfiConventionalMemory
            if (desc->pageCount * 4096 > largestFreeSegmentSize) {
                largestFreeSegment = desc->physicalAddress;
                largestFreeSegmentSize = desc->pageCount * 4096;
            }
        }
    }

    usedMemory = Memory::size();
    pageFrames.setSize(usedMemory / 0x8000 + 1);
    pageFrames.setBuffer(largestFreeSegment);
    memset((unsigned char *)largestFreeSegment, 0xFF, pageFrames.size());

    for (int i = 0; i < bInfo.memmap.descCount; ++i) {
        EFIMemoryDescriptor *desc = (EFIMemoryDescriptor *)((unsigned long long)bInfo.memmap.base_addr + i * bInfo.memmap.descSize);
        if (desc->type == 7) { // EfiConventionalMemory
            free(desc->physicalAddress, desc->pageCount);
        } else {
            usedMemory -= 4096 * desc->pageCount;
        }
    }
    lock(largestFreeSegment, pageFrames.size() / 4096 + 1);

    unsigned long long _kernelSize = (unsigned long long)&_kernelEnd - (unsigned long long)&_kernelStart;
    unsigned long long _kernelPages = (unsigned long long)_kernelSize / 4096 + 1;
    lock(&_kernelStart, _kernelPages);

    LOG_DEBUG_OK();
}

void PageFrameAllocator::free(void *address, unsigned long long count) {
    for (int i = 0; i < count; i++) {
        if (pageFrames[(unsigned long long)address / 4096 + i]) {
            freeMemory += 4096; usedMemory -= 4096;
        }
        pageFrames.set((unsigned long long)address / 4096 + i, false);
    }
}

void PageFrameAllocator::lock(void *address, unsigned long long count) {
    for (int i = 0; i < count; i++) {
        if (!pageFrames[(unsigned long long)address / 4096 + i]) {
            freeMemory -= 4096; usedMemory += 4096;
        }
        pageFrames.set((unsigned long long)address / 4096 + i, true);
    }
}

void *PageFrameAllocator::alloc() {
    for (unsigned long long i = 0; i < pageFrames.size() * 8; i++) {
        if (pageFrames[i] == true) continue;
        lock((void *)(i * 4096), 1);
        return (void *)(i * 4096);
    }

    return nullptr;
}

void *PageFrameAllocator::alloc(unsigned long long count) {
    for (unsigned long long i = 0; i < pageFrames.size() * 8; i++) {
        bool is_suitable = true;
        for (unsigned long long j = 0; j < count; j++) {
            if (pageFrames[i + j] == true) {
                is_suitable = false;
                break;
            };
        }
        if (!is_suitable) continue;
        lock((void *)(i * 4096), count);
        return (void *)(i * 4096);
    }

    return nullptr;
}

unsigned long long PageFrameAllocator::get_free() {
    return freeMemory;
}

unsigned long long PageFrameAllocator::get_used() {
    return usedMemory;
}