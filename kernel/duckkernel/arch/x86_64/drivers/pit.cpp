#include <drivers/pit.h>
#include <drivers/ports.h>
#include <interrupts/idt.h>
#include <utils/logger.h>

void PIT::init() {
#if KDEBUG == true
    Logger::info("(PIT)");
#endif
    asm volatile ("cli");    
    IDTManager::register_handler(0x20, tick);
    unsigned short divisor = 0x400;
    outb(0x43, 0x36);
    io_wait();
    unsigned char l = (unsigned char)(divisor & 0xFF);
    unsigned char h = (unsigned char)( (divisor>>8) & 0xFF );
    outb(0x40, l);
    io_wait();
    outb(0x40, h);
    io_wait();
    asm volatile ("sti");
}

__attribute__((interrupt)) 
void tick(InterruptFrame* frame) {
    PICManager::end_master();
    Timer::tick(); 
}