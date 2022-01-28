#pragma once

typedef struct {
    unsigned short size;
    unsigned long long offset;
} __attribute__((packed)) GDTR;

typedef struct {
    unsigned short limit0;
    unsigned short base0;
    unsigned char base1;
    unsigned char access;
    unsigned char flags;
    unsigned char base2;
} __attribute__((packed)) SegmentDescriptor;

typedef struct {
    SegmentDescriptor null;
    SegmentDescriptor kernel_code;
    SegmentDescriptor kernel_data;
    SegmentDescriptor user_code;
    SegmentDescriptor user_data;
    SegmentDescriptor tss;
} __attribute__((aligned(0x1000))) 
  __attribute__((packed)) GDT;

typedef struct {
    unsigned int r0;
    unsigned long long rsp0;
    unsigned long long rsp1;
    unsigned long long rsp2;
    unsigned long long r1;
    unsigned long long ist1;
    unsigned long long ist2;
    unsigned long long ist3;
    unsigned long long ist4;
    unsigned long long ist5;
    unsigned long long ist6;
    unsigned long long ist7;
    unsigned long long r2;
    unsigned short r3;
    unsigned short iomap_base;
} __attribute__((aligned(0x1000))) 
  __attribute__((packed)) TSS;

class GDTManager {
public:
    static void init();
};

extern GDT globalDT;
extern TSS taskSS;
extern "C" void loadGDT(GDTR* gdtr);
extern "C" void loadTSS();