#include <memory/memory.h>
#include <memory/heap.h>
#include <memory/pageframe.h>
#include <utils/logger.h>

unsigned long long Memory::m_size = 0;

void Memory::init() {
    LOG_DEBUG("Calculating memory size...");
    for (unsigned long long i = 0; i < bInfo.memmap.descCount; ++i) {
        EFIMemoryDescriptor *descriptor = (EFIMemoryDescriptor *)((char*)bInfo.memmap.base_addr + (i * bInfo.memmap.descSize));
        m_size += descriptor->pageCount * 4096;
    }
    LOG_DEBUG_OK();
    PageFrameAllocator::init();
    Heap::k.init();
}

unsigned long long Memory::size() {
    return m_size;
}
