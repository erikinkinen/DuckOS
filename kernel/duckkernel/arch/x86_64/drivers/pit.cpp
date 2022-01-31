#include <drivers/pit.h>
#include <drivers/ports.h>
#include <interrupts/idt.h>
#include <utils/logger.h>
#include <task.h>

void PIT::init() {
    LOG_DEBUG("(PIT)...");
    unsigned short divisor = 0x1000;
    outb(0x43, 0x36);
    io_wait();
    unsigned char l = (unsigned char)(divisor & 0xFF);
    unsigned char h = (unsigned char)( (divisor>>8) & 0xFF );
    outb(0x40, l);
    io_wait();
    outb(0x40, h);
    io_wait();
}
extern "C" {
void tick(InterruptFrame* frame, unsigned long long rax) {
    PICManager::end_master();
    Timer::tick(); 
}
}