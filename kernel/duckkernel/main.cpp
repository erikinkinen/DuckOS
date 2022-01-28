#include <arch/arch.h>
#include <init/boot.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <fs/fsmanager.h>
#include <memory/memory.h>
#include <memory/heap.h>
#include <utils/logger.h>
#include <utils/utils.h>
#include <task.h>

BootInfo bInfo;

extern unsigned long long _kernelStart;
extern unsigned long long _kernelEnd;

extern "C" {

int _start(BootInfo *_bInfo) {
    bInfo = *_bInfo;

#if KDEBUG == true
    SerialPort com1(1);
    Logger::init(&com1);
    put_rect(0x444444, 0, bInfo.fb.height - 10, bInfo.fb.width, 10);
#endif

    Arch::preinit();
    Memory::init();
    Arch::init();

    Timer::init();
    TaskManager::init();  
    FSManager::init();  

#if KDEBUG == true
    Logger::report();
    put_rect(0xFFFFFF, 0, bInfo.fb.height - 10, bInfo.fb.width / 10, 10);
#endif

    long long stdin = FSManager::open("/dev/console", 4);
    long long stdout = FSManager::open("/dev/console", 2);
    long long stderr = FSManager::open("/dev/console", 2);
    
    TaskManager::user_exec("/init", nullptr, nullptr);

    Logger::error("Init did not start!");
    for (;;);
} 
}
