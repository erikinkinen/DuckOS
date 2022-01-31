#pragma once

#include <drivers/pic.h>

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_TrapGate         0b10001111
#define IDT_TA_UserTrapGate     0b11101111

typedef struct {
    unsigned short size;
    unsigned long long offset;
} __attribute__((packed)) IDTR;

typedef struct {
    unsigned short offset0;
    unsigned short selector;
    unsigned char ist;
    unsigned char type_attr;
    unsigned short offset1;
    unsigned int offset2;
    unsigned int reserved;
} __attribute__((packed)) InterruptDescriptor;

typedef struct
{
    unsigned long long ip;
    unsigned long long cs;
    unsigned long long flags;
    unsigned long long sp;
    unsigned long long ss;
} InterruptFrame;

typedef struct
{
    unsigned long long r15;
    unsigned long long r14;
    unsigned long long r13;
    unsigned long long r12;
    unsigned long long r11;
    unsigned long long r10;
    unsigned long long r9;
    unsigned long long r8;
    unsigned long long rbp;
    unsigned long long rdi;
    unsigned long long rsi;
    unsigned long long rdx;
    unsigned long long rcx;
    unsigned long long rbx;
    unsigned long long rax;
    unsigned long long int_no;
    unsigned long long err_no;
    InterruptFrame frame;
} InterruptRegisters;

class IDTManager {
public:
    static void init();
    static void register_handler(unsigned char int_no, void (*handler)(InterruptFrame *));
    static InterruptRegisters *isr_handler(InterruptRegisters *regs);
    static void syscall_handler(InterruptRegisters *regs);
private:
    static void set_offset(InterruptDescriptor *descriptor, unsigned long long offset);
    static const char *error_msgs[];
};