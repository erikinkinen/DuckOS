#pragma once

#include <memory/paging.h>
#include <memory/pageframe.h>
#include <utils/utils.h>

class PageTableManager {
public:
    PageTableManager();
    void map(void *virtualAddress, void *physicalAddress, bool readWrite, bool user);
    void enable();
    PageTable *pml4;

    static void init();
    static PageTableManager *k;
};
