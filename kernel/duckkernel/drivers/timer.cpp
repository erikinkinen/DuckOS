#include <drivers/timer.h>
#include <utils/logger.h>
#include <task.h>

void Timer::init() {
#if KDEBUG == true
    Logger::info("Initializing timer ");
#endif
    TIMER::init();
#if KDEBUG == true
    Logger::ok();
#endif
}

void Timer::tick() {
    TaskManager::task_switch();
}