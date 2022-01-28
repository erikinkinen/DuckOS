#pragma once

#include <fs/fsmanager.h>

class InitrdFile : public AbstractFile {
public:
    InitrdFile(FSNode *parent);
    unsigned long long get_size() override;
    signed long long read(void *buf, unsigned long long nbytes);
    signed long long write(const void *buf, unsigned long long nbytes);
    unsigned char *base;
    unsigned long long position;
    unsigned long long size;
};

class Initrd : public AbstractFS {
public:
    Initrd(FSNode *parent);
    unsigned long long oct2bin(unsigned char *octstr, unsigned long long size);
};
