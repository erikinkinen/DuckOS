#include <memory/pagetable.h>

PageTableManager *PageTableManager::k = nullptr;

#include <drivers/serial.h>
#include <utils/utils.h>
#include <utils/logger.h>

extern SerialPort *com1_ptr;
extern char buffer[256];

PageTableManager::PageTableManager() {
    pml4 = (PageTable *)PageFrameAllocator::alloc();
    memset(pml4, 0, 4096);
}

void PageTableManager::enable() {
    asm volatile ("mov %0, %%cr3" : : "r" (pml4));
}

void PageTableManager::init() {
    LOG_DEBUG("Initializing paging...");

    k = new PageTableManager();
    for (unsigned long long i = 0; i < Memory::size(); i += 4096) {
        k->map((void *)i, (void *)i, true, false);
    }

    for (unsigned long long i = (unsigned long long)bInfo.fb.base_addr; 
        i < (unsigned long long)bInfo.fb.base_addr + bInfo.fb.size + 0x1000; 
        i += 4096)
    {
        k->map((void*)i, (void*)i, true, false);
    }

    k->enable();
    LOG_DEBUG_OK();
}

void PageTableManager::map(void *virtualAddress, void *physicalAddress, bool readWrite, bool user) {
    unsigned long long pdp_i, pd_i, pt_i, p_i;
    p_i = ((unsigned long long)virtualAddress >> 12) & 0x1ff;
    pt_i = ((unsigned long long)virtualAddress >> 21) & 0x1ff;
    pd_i = ((unsigned long long)virtualAddress >> 30) & 0x1ff;
    pdp_i = ((unsigned long long)virtualAddress >> 39) & 0x1ff;
    PageDirectoryEntry pde;
    
    pde = pml4->entries[pdp_i];
    PageTable *pdp;
    if (pde.present) {
        pdp = (PageTable *)((unsigned long long)pde.address << 12);
    } else {
        pdp = (PageTable *)PageFrameAllocator::alloc();
        memset(pdp, 0, 4096);
        pde.address = (unsigned long long)pdp >> 12;
        pde.present = true;
        pde.readWrite = true;
        pde.userSuper = true;
        pml4->entries[pdp_i] = pde;
    }

    pde = pdp->entries[pd_i];
    PageTable *pd;
    if (pde.present) {
        pd = (PageTable *)((unsigned long long)pde.address << 12);
    } else {
        pd = (PageTable *)PageFrameAllocator::alloc();
        memset(pd, 0, 4096);
        pde.address = (unsigned long long)pd >> 12;
        pde.present = true;
        pde.readWrite = true;
        pde.userSuper = true;
        pdp->entries[pd_i] = pde;
    }

    pde = pd->entries[pt_i];
    PageTable *pt;
    if (pde.present) {
        pt = (PageTable *)((unsigned long long)pde.address << 12);
    } else {
        pt = (PageTable *)PageFrameAllocator::alloc();
        memset(pt, 0, 4096);
        pde.address = (unsigned long long)pt >> 12;
        pde.present = true;
        pde.readWrite = true;
        pde.userSuper = true;
        pd->entries[pt_i] = pde;
    }

    pde = pt->entries[p_i];
    pde.address = (unsigned long long)physicalAddress >> 12;
    pde.present = true;
    pde.readWrite = readWrite;
    pde.userSuper = user;
    pt->entries[p_i] = pde;
}
