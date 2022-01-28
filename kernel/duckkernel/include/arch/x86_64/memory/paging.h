#pragma once

typedef struct {
    bool present : 1;
    bool readWrite : 1;
    bool userSuper : 1;
    bool writeThrough : 1;
    bool cacheDisabled : 1;
    bool accessed : 1;
    bool ignore0 : 1; 
    bool largerPages : 1;
    bool ignore1 : 1;
    unsigned char available : 3;
    unsigned long long address : 52;
} PageDirectoryEntry;

typedef struct { 
    PageDirectoryEntry entries[512];
} __attribute__((aligned(0x1000))) PageTable;