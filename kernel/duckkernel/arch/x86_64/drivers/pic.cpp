#include <drivers/pic.h>
#include <drivers/ports.h>
#include <utils/logger.h>

void PICManager::init() {
#if KDEBUG == true
    Logger::info("Initializing PIC...");
#endif

    unsigned char a1, a2; 

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, 0);
    io_wait();
    outb(PIC2_DATA, 0); 
    io_wait();

    outb(PIC1_DATA, 0b11111110);
    io_wait();
    outb(PIC2_DATA, 0b11111111);
    io_wait();

#if KDEBUG == true
    Logger::ok();
#endif
}

void PICManager::end_master() {
    outb(PIC1_COMMAND, PIC_EOI);
}

void PICManager::end_slave() {
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}