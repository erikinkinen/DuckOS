#include <memory/gdt.h>
#include <utils/logger.h>

TSS taskSS = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

__attribute__((aligned(0x1000)))
GDT globalDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // null
    {0, 0, 0, 0x9a, 0x20, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0x20, 0}, // kernel data segment
    {0, 0, 0, 0xFa, 0x20, 0}, // user code segment
    {0, 0, 0, 0xF2, 0x20, 0}, // user data segment
    {0, 0, 0, 0x89, 0x20, 0}, // tss
};

GDTR gdtr = {
    .size = sizeof(GDT) - 1,
    .offset = (unsigned long long)&globalDT,
};

void GDTManager::init() {
    LOG_DEBUG("Initializing GDT...");
    globalDT.tss.base0 = (unsigned long long)(&taskSS) & 0xFFFF;
    globalDT.tss.base1 = ((unsigned long long)(&taskSS) >> 16) & 0xFF;
    globalDT.tss.base2 = ((unsigned long long)(&taskSS) >> 24) & 0xFF;
    globalDT.tss.limit0 = 103;
    loadGDT(&gdtr);
    loadTSS();
    LOG_DEBUG_OK();
}