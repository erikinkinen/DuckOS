#include <fs/devfs.h>
#include <fs/initrd.h>
#include <utils/logger.h>

InitrdFile::InitrdFile(FSNode *parent)
    : AbstractFile(parent) {}

unsigned long long InitrdFile::get_size() {
    return size;
}

signed long long InitrdFile::read(void *buf, unsigned long long nbytes) {
    if (position >= size)
        return 0;

    unsigned char *ptr = base + position;
    for (int i = 0; i < nbytes; i++) {
        ((char*)buf)[i] = *(base + position);
        position ++;
        if (position >= size)
            return i;
    }

    return nbytes;
}

signed long long InitrdFile::write(const void *buf, unsigned long long nbytes) {
    return -1;
}

static char *devfsname = (char*)"dev";

Initrd::Initrd(FSNode *parent) : AbstractFS(parent) {
    unsigned char *ptr = (unsigned char *)bInfo.initrd;
    while (ptr + 512 <= (unsigned char *)bInfo.initrd + bInfo.initrd_size) {
        if (*ptr == '\0') break;
        InitrdFile *file = new InitrdFile(this);
        char *filename = (char*)Heap::k.alloc(101);
        for (int i = 0; i < 100; i++) {
            filename[i] = ptr[i];
        }
        filename[100] = 0;
        file->name = filename;
        file->position = 0;
        file->size = oct2bin(ptr + 0x7c, 11);
        file->base = ptr + 512;
        children->append(file);
        ptr += (file->size / 512 + 2) * 512;
    }

    DevFS *devfs = new DevFS(this);
    devfs->name = devfsname;
    children->append(devfs);
}

unsigned long long Initrd::oct2bin(unsigned char *octstr, unsigned long long size) {
    unsigned long long n = 0;
    unsigned char *c = octstr;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}