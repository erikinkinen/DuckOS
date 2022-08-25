#include <arch/arch.h>
#include <drivers/pit.h>
#include <interrupts/idt.h>
#include <memory/gdt.h>
#include <memory/pagetable.h>

void Arch::preinit() {
    GDTManager::init();
}

void Arch::init() {
    PageTableManager::init();
    PIT::init();
    IDTManager::init();
}