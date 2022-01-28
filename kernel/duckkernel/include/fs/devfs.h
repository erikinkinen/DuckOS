#pragma once

#include <fs/fsmanager.h>

class ConsoleFile : public AbstractFile {
public:
    ConsoleFile(FSNode *parent);
    unsigned long long get_size() override;
    signed long long write(const void *buf, unsigned long long nbytes) override;
    signed long long read(void *buf, unsigned long long nbytes) override;
    unsigned long long color = 0xFFFFFFFF;
    static const char *default_name;
};

class DevFS : public AbstractFS {
public:
    DevFS(FSNode *parent);
};