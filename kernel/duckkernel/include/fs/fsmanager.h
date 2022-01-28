#pragma once

#include <memory/heap.h>

typedef struct {
    unsigned long long dev;
    unsigned long long ino;
    unsigned long long mode;
    unsigned long long nlink;
    unsigned long long uid;
    unsigned long long gid;
    unsigned long long rdev;
    unsigned long long size;
    unsigned long long blksize;
    unsigned long long blocks;
    unsigned long long atime;
    unsigned long long mtime;
    unsigned long long ctime;
} Stat;

class FSNode {
public:
    FSNode(FSNode *parent, unsigned long long type);
    virtual unsigned long long get_size();
    char *name;
    FSNode *next;
    FSNode *parent;
    unsigned long long type;
    unsigned long long id;
    static unsigned long long next_id;
};

class FSNodeList {
public:
    FSNodeList();
    void append(FSNode *node);
    FSNode *first = nullptr;
    FSNode *last = nullptr;
};

class AbstractFile : public FSNode {
public:
    AbstractFile(FSNode *parent);
    virtual signed long long write(const void *buf, unsigned long long nbytes) = 0;
    virtual signed long long read(void *buf, unsigned long long nbytes) = 0;
};

class AbstractDirectory : public FSNode {
public:
    AbstractDirectory(FSNode *parent);
    AbstractDirectory(FSNode *parent, unsigned long long type);
    FSNode *node_from_path(const char *path);
public:
    FSNodeList *children;
};

class AbstractFS : public AbstractDirectory {
public:
    AbstractFS(FSNode *parent);
};

class FileDescriptor {
public:
    long long id;
    AbstractFile *node;
    unsigned long long mode;
    FileDescriptor *next;
};

class FDTable {
public:
    FDTable();
    FileDescriptor *first;
    FileDescriptor *last;
    unsigned long long next_id = 0;
};

class FSManager {
public:
    static void init();
    static signed long long open(const char *path, unsigned long long mode);
    static signed long long write(long long fd, const void *buf, unsigned long long nbytes);
    static signed long long read(long long fd, void *buf, unsigned long long nbytes);
    static signed long long stat(const char *path, Stat *buf);
    static FSNode *node_from_path(const char *path);
private:
    static AbstractFS *root;
};
