#include <arch/arch.h>
#include <init/boot.h>
#include <drivers/serial.h>
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

int kernel_main(BootInfo *_bInfo) {
    bInfo = *_bInfo;
    LOG_INIT();

    Arch::preinit();
    Memory::init();
    Arch::init();

    TaskManager::init();  
    FSManager::init();  

    long long stdin = FSManager::open("/dev/console", 4);
    long long stdout = FSManager::open("/dev/console", 2);
    long long stderr = FSManager::open("/dev/console", 2);

    LOG_MEM_REPORT();
    
    TaskManager::user_exec("/init", nullptr, nullptr);

    Logger::error("Init did not start!");
    return 0;
} 
}
