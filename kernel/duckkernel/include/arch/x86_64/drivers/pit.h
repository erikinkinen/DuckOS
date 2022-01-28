#pragma once

#include <drivers/timer.h>
#include <interrupts/idt.h>

__attribute__((interrupt)) static void tick(InterruptFrame* frame);

class PIT {
public:
    static void init();
};
