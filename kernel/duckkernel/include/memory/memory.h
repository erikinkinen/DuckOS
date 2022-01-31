#pragma once

#include <init/boot.h>

typedef struct {
    unsigned int type;
    void *physicalAddress;
    void *virtualAddress;
    unsigned long long pageCount;
    unsigned long long attributes;
} EFIMemoryDescriptor;

class Memory {
public:
    static void init();
    static unsigned long long size();
private:
    static unsigned long long m_size;
};
