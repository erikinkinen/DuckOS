#include <interrupts/idt.h>
#include <utils/logger.h>
#include <memory/gdt.h>
#include <memory/pageframe.h>
#include <fs/fsmanager.h>
#include <task.h>

IDTR idtr;

extern "C" {
    extern unsigned long long isr_handlers[];
}

const char *IDTManager::error_msgs[0x20] = {
    "Division by zero",
    "Debug",
    nullptr,
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "CSO",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 error",
    "Alignment check",
    "Machine check",
    "SIMD error",
    "Virtualization exception",
    "Control protection exception",
    "Reserved",
    "Hypervisor Injection exception",
    "VMM communiction exception",
    "Security exception",
    "Resercved"
};

void IDTManager::init() {
    LOG_DEBUG("Initializing IDT...");
    asm volatile ("cli");
    idtr.size = 0xFFF;
    idtr.offset = (unsigned long long)PageFrameAllocator::alloc();
    memset((char *)idtr.offset, 0, 4096);

    void *stack =  (void *)((unsigned long long)PageFrameAllocator::alloc() + 0xFFF);
    taskSS.rsp0 = (unsigned long long) stack;
    void *stack1 =  (void *)((unsigned long long)PageFrameAllocator::alloc() + 0xFFF);
    taskSS.ist1 = (unsigned long long) stack1;

    for (int int_no = 0; int_no < 256; int_no++) {
        InterruptDescriptor *int_d = (InterruptDescriptor *)(idtr.offset + int_no * sizeof(InterruptDescriptor));
        set_offset(int_d, isr_handlers[int_no]);
        if (int_no >= 0x80) int_d->type_attr = IDT_TA_UserTrapGate;
        else int_d->type_attr = IDT_TA_InterruptGate;
        int_d->selector = 8;
        if (int_no < 0x20) int_d->ist = 1;
    }

    asm volatile ("lidt %0" : : "m" (idtr));
    LOG_DEBUG_OK();
    
    PICManager::init();
}

void IDTManager::set_offset(InterruptDescriptor *descriptor, unsigned long long offset) {
    descriptor->offset0 = (unsigned short)(offset & 0xFFFF);
    descriptor->offset1 = (unsigned short)((offset >> 16) & 0xFFFF);
    descriptor->offset2 = (unsigned int)((offset >> 32) & 0xFFFFFFFF);
}

InterruptRegisters *IDTManager::isr_handler(InterruptRegisters *regs) {
    if (regs->int_no < 0x20) {
        Logger::error("\nException: ");
        Logger::error(error_msgs[regs->int_no]);
        if (regs->int_no == 0xE) {
            Logger::error(" @ 0x");
            unsigned long long cr2;
            asm volatile("movq %%cr2, %0": "=r"(cr2));
            Logger::error(itoa(cr2, buffer, 16));
        }
        Logger::error("\nError code: 0x");
        Logger::error(itoa(regs->err_no, buffer, 16));
        Logger::error("\nRIP: 0x");
        Logger::error(itoa(regs->frame.ip, buffer, 16));
        Logger::error("\nCS: 0x");
        Logger::error(itoa(regs->frame.cs, buffer, 16));
        Logger::error("\nFLAGS: 0x");
        Logger::error(itoa(regs->frame.flags, buffer, 16));
        Logger::error("\nRSP: 0x");
        Logger::error(itoa(regs->frame.sp, buffer, 16));
        Logger::error("\nSS: 0x");
        Logger::error(itoa(regs->frame.ss, buffer, 16));
        for (;;) { asm volatile ("cli"); asm volatile ("hlt"); }
    }

    if (regs->int_no >= 0x20 && regs->int_no < 0x28) PICManager::end_master();
    else if (regs->int_no < 0x30) PICManager::end_slave();

    if (regs->int_no == 0x20) {
        return TaskManager::task_switch(regs);
    }

    if (regs->int_no == 0x80) {
        syscall_handler(regs);
    }
    return regs;
}

void IDTManager::syscall_handler(InterruptRegisters *regs) {
    unsigned long long fn = regs->rax;
    unsigned long long a1 = regs->rdi;
    unsigned long long a2 = regs->rsi;
    unsigned long long a3 = regs->rdx;

    if (fn == 0x0) {
        regs->rax = FSManager::read(a1, (void *)a2, a3);
    } else if (fn == 0x1) {
        regs->rax = FSManager::write(a1, (const void *)a2, a3);
    } else if (fn == 0x2) {
        regs->rax = FSManager::stat((const char *)a1, (Stat *)a2);
    } else if (fn == 0x3) {
        regs->rax = FSManager::open((const char *)a1, a2);
    } else if (fn == 0x100) {
        TaskManager::fork(regs);
    } else if (fn == 0x101) {
        regs->rax = TaskManager::getpid();
    } else regs->rax = -1;
}
