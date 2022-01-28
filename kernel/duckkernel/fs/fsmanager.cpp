#include <fs/devfs.h>
#include <fs/fsmanager.h>
#include <fs/initrd.h>
#include <task.h>
#include <utils/logger.h>

AbstractFS *FSManager::root;
unsigned long long FSNode::next_id = 0;

FSNode::FSNode(FSNode *parent, unsigned long long type) : parent(parent), type(type) {
    id = FSNode::next_id++;
}

unsigned long long FSNode::get_size() {
    return 0;
}

FSNodeList::FSNodeList() : 
    first(nullptr), last(nullptr) {}

void FSNodeList::append(FSNode *node) {
    if (last) {
        last->next = node;
        last = node;
    } else last = first = node;
}

AbstractFile::AbstractFile(FSNode *parent) : FSNode(parent, 0) {}

AbstractDirectory::AbstractDirectory(FSNode *parent) : FSNode(parent, 1) {
    children = new FSNodeList();
}

AbstractDirectory::AbstractDirectory(FSNode *parent, unsigned long long type) : FSNode(parent, type) {
    children = new FSNodeList();
}

AbstractFS::AbstractFS(FSNode *parent) : AbstractDirectory(parent, 2) {}

FSNode *AbstractDirectory::node_from_path(const char *path) {
    FSNode *cur = children->first;
    while (cur) {
        for (unsigned long long i = 0;;i++) {
            if (path[i] == '/' && cur->name[i] == 0) {
                if (path[i+1]) {
                    if (cur->type == 0) return (FSNode*)0;
                    else return ((AbstractDirectory *)cur)->node_from_path(path + i + 1);
                }
                else return cur;
            } else if (path[i] == cur->name[i]) {
                if (path[i] == 0) return cur;
                else continue;
            } else break;
        }
        cur = cur->next;
    }
    return (FSNode*)0;
}

FDTable::FDTable() : 
    first(nullptr), last(nullptr) {}

void FSManager::init() {
#if KDEBUG == true
    Logger::info("Initializing file system support...");
#endif
    root = new Initrd(nullptr);
#if KDEBUG == true
    Logger::ok();
#endif
}

signed long long FSManager::open(const char *path, unsigned long long mode) {
    if (!TaskManager::current_task) return -1;
    if (!TaskManager::current_task->fd_table) return -1;
    FSNode *node = node_from_path(path);
    if (!node) return -1;
    if (node->type != 0) return -1;
    FileDescriptor *fd = new FileDescriptor();
    fd->id = TaskManager::current_task->fd_table->next_id++;
    fd->node = (AbstractFile*)node;
    fd->mode = mode & 0x6;
    fd->next = nullptr;
    if (TaskManager::current_task->fd_table->last) {
        TaskManager::current_task->fd_table->last->next = fd;
        TaskManager::current_task->fd_table->last = fd;
    }
    else
        TaskManager::current_task->fd_table->last =
        TaskManager::current_task->fd_table->first = fd;
    return fd->id;
}

signed long long FSManager::write(long long fd, const void *buf, unsigned long long nbytes) {
    if (!TaskManager::current_task) return -1;
    if (!TaskManager::current_task->fd_table) return -1;
    FileDescriptor *fdstruct = TaskManager::current_task->fd_table->first;
    if (!fdstruct) return -1;
    while (fdstruct->id != fd) {
        fdstruct = fdstruct->next;
        if (!fdstruct) return -1;
    }
    if ((fdstruct->mode & 2) != 2) return -1;
    if (!fdstruct->node) return -1;
    if (fdstruct->node->type != 0) return -1;
    return fdstruct->node->write(buf, nbytes);
}

signed long long FSManager::read(long long fd, void *buf, unsigned long long nbytes) {
    if (!TaskManager::current_task) return -1;
    if (!TaskManager::current_task->fd_table) return -2;
    FileDescriptor *fdstruct = TaskManager::current_task->fd_table->first;
    if (!fdstruct) return -3;
    while (fdstruct->id != fd) {
        fdstruct = fdstruct->next;
        if (!fdstruct) return -4;
    }
    if ((fdstruct->mode & 4) != 4) return -5;
    if (!fdstruct->node) return -6;
    if (fdstruct->node->type != 0) return -7;
    return fdstruct->node->read(buf, nbytes);
}

signed long long FSManager::stat(const char *path, Stat *buf) {
    if (!TaskManager::current_task) return -1;
    if (!TaskManager::current_task->fd_table) return -1;
    FSNode *node = node_from_path(path);
    if (!node) return -1;
    buf->dev = 1;
    buf->ino = node->id;
    buf->mode = (node->type << 16) & 0x17;
    buf->nlink = 0;
    buf->uid = 0;
    buf->gid = 0;
    buf->rdev = 0;
    buf->size = node->get_size();
    buf->blksize = 512;
    buf->blocks = node->get_size() / 512 + 1;
    buf->atime = 0;
    buf->mtime = 0;
    buf->ctime = 0;
    return 0;
}

FSNode *FSManager::node_from_path(const char *path) {
    if (*path == '/') return root->node_from_path(path + 1);
    else return (FSNode*)0;
}
