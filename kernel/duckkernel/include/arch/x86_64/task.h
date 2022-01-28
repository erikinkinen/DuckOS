#pragma once

#include <fs/fsmanager.h>
#include <memory/paging.h>
#include <memory/pagetable.h>

typedef struct {
   unsigned char magic[4];
   unsigned char bitversion;
   unsigned char endianness;
   unsigned char header_version;
   unsigned char abi;
   unsigned char unused0[8];
   unsigned short type;
   unsigned short inst_set;
   unsigned int elf_version;
   unsigned long long entry_pos;
   unsigned long long phdr_pos;
   unsigned long long shdr_pos;
   unsigned int flags;
   unsigned short header_size;
   unsigned short phdr_entry_size;
   unsigned short phdr_entry_count;
   unsigned short shdr_entry_size;
   unsigned short shdr_entry_count;
   unsigned short index;   
} ElfHeader;

typedef struct {
   unsigned int type;
   unsigned int flags;
   unsigned long long p_offset;
   unsigned long long p_vaddr;
   unsigned long long unused0;
   unsigned long long p_filesz;
   unsigned long long p_memsz;
   unsigned long long align;
} ElfProgramHeader;

typedef struct {
	unsigned long long sp;        /* 0 */
	unsigned long long bp;        /* 8 */
	unsigned long long ip;        /* 16 */
	unsigned long long tls_base;  /* 24 */
	unsigned long long saved[5]; 
} KThreadContext;

typedef struct {
	KThreadContext context;
	unsigned char fp_regs[512] __attribute__((aligned(16)));
	PageTableManager *ptm;
} Thread;

struct Task;
struct Task
{
   long long id;
   Thread thread;
   FDTable *fd_table;
   Task *next;
};
typedef struct Task Task;

class TaskManager {
public:
   static void init();
   static void task_switch();
   static long long fork();
   static void user_exec(char const *path, char const **argv,  char const **envp);
   static void move_stack(void *newstack);
   static long long getpid();
   static volatile Task *current_task;
private:
   static volatile Task *ready_queue;
   static long long next_pid;
};