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

__attribute__((interrupt)) static void pagefault_handler(InterruptFrame* frame);
__attribute__((interrupt)) static void doublefault_handler(InterruptFrame* frame);
__attribute__((interrupt)) static void gpfault_handler(InterruptFrame* frame);
extern "C" {
    extern __attribute__((interrupt)) void syscall_handler(InterruptFrame* frame);
    unsigned long long syscall_handler_c(unsigned long long fn, unsigned long long a1, unsigned long long a2, unsigned long long a3, unsigned long long a4, unsigned long long a5);
}

class IDTManager {
public:
    static void init();
    static void register_handler(unsigned char int_no, void (*handler)(InterruptFrame *));
private:
    static void set_offset(InterruptDescriptor *descriptor, unsigned long long offset);
};