#include <memory/memory.h>
#include <memory/heap.h>
#include <memory/pageframe.h>
#include <drivers/serial.h>
#include <utils/logger.h>
#include <utils/utils.h>

unsigned long long Memory::m_size = 0;

void Memory::init() {
#if KDEBUG == true
    Logger::info("Calculating memory size...");
#endif

    for (unsigned long long i = 0; i < bInfo.memmap.descCount; ++i) {
        efi_mem_desc_t *descriptor = (efi_mem_desc_t *)((char*)bInfo.memmap.base_addr + (i * bInfo.memmap.descSize));
        m_size += descriptor->pageCount * 4096;
    }

#if KDEBUG == true
    Logger::ok();
#endif

    PageFrameAllocator::init();
    Heap::k.init();
}

unsigned long long Memory::size() {
    return m_size;
}
