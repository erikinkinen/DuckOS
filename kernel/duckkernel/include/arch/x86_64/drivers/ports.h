#pragma once

static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) );
}

static inline unsigned char inb(unsigned short port) {
    unsigned char value;
    asm volatile ( "inb %1, %0" : "=a"(value) : "Nd"(port) );
    return value;
}

static inline void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}