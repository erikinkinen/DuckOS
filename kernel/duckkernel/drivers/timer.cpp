#include <drivers/timer.h>
#include <utils/logger.h>

void Timer::init() {
    LOG_DEBUG("Initializing timer ");
    TIMER::init();
    LOG_DEBUG_OK();
}

void Timer::tick() {}