#include <interrupts/idt.h>
#include <utils/logger.h>
#include <memory/gdt.h>
#include <memory/pageframe.h>
#include <fs/fsmanager.h>

IDTR idtr;

void IDTManager::init() {
#if KDEBUG == true
    Logger::info("Initializing IDT...");
#endif
    asm volatile ("cli");
    idtr.size = 0xFFF;
    idtr.offset = (unsigned long long)PageFrameAllocator::alloc();
    memset((char *)idtr.offset, 0, 4096);

    void *stack =  (void *)((unsigned long long)PageFrameAllocator::alloc() + 0xF00);
    taskSS.rsp0 = (unsigned long long) stack;
    void *stack1 =  (void *)((unsigned long long)PageFrameAllocator::alloc() + 0xF00);
    taskSS.ist1 = (unsigned long long) stack1;

    register_handler(0x8, doublefault_handler);
    register_handler(0xD, gpfault_handler);
    register_handler(0xE, pagefault_handler);
    register_handler(0x80, syscall_handler);

    asm volatile ("lidt %0" : : "m" (idtr));
#if KDEBUG == true
    Logger::ok();
#endif

    PICManager::init();
    asm volatile ("sti");
}

void IDTManager::register_handler(unsigned char int_no, void (*handler)(InterruptFrame *)) {
    InterruptDescriptor *int_d = (InterruptDescriptor *)(idtr.offset + int_no * sizeof(InterruptDescriptor));
    set_offset(int_d, (unsigned long long)handler);
    if (int_no >= 0x80) int_d->type_attr = IDT_TA_UserTrapGate;
    else int_d->type_attr = IDT_TA_InterruptGate;
    int_d->selector = 8;
    if (int_no < 0x20) int_d->ist = 1;
}

void IDTManager::set_offset(InterruptDescriptor *descriptor, unsigned long long offset) {
    descriptor->offset0 = (unsigned short)(offset & 0xFFFF);
    descriptor->offset1 = (unsigned short)((offset >> 16) & 0xFFFF);
    descriptor->offset2 = (unsigned int)((offset >> 32) & 0xFFFFFFFF);
}

__attribute__((interrupt))
void pagefault_handler(InterruptFrame *frame){
    asm volatile ("cli");
    Logger::error("\r\n!!! PAGE FAULT !!!\r\n");
    for (;;) { asm volatile ("cli"); asm volatile ("hlt"); }
}

__attribute__((interrupt))
void doublefault_handler(InterruptFrame *frame){
    asm volatile ("cli");
    Logger::error("\r\n!!! DOUBLE FAULT !!!\r\n");
    for (;;) { asm volatile ("cli"); asm volatile ("hlt"); }
}

__attribute__((interrupt))
void gpfault_handler(InterruptFrame *frame){
    asm volatile ("cli");
    Logger::error("\r\n!!! GENERAL PROTECTION FAULT !!!\r\n");
    for (;;) { asm volatile ("cli"); asm volatile ("hlt"); }
}

unsigned long long syscall_handler_c(unsigned long long fn, unsigned long long a1, unsigned long long a2, unsigned long long a3, unsigned long long a4, unsigned long long a5){
    if (fn == 0) {
        return FSManager::read(a1, (void *)a2, a3);
    } else if (fn == 1) {
        return FSManager::write(a1, (const void *)a2, a3);
    } else if (fn == 2) {
        return FSManager::stat((const char *)a1, (Stat *)a2);
    } else if (fn == 3) {
        return FSManager::open((const char *)a1, a2);
    }
    return -1;
}