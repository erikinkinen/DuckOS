#pragma once

#include <fs/fsmanager.h>
#include <interrupts/idt.h>
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

typedef struct ProcessImageMap ProcessImageMap;
struct ProcessImageMap {
   void *vaddr;
   void *paddr;
   unsigned long long size;
   ProcessImageMap *next;
};

struct Task;
struct Task
{
   long long id;
	PageTableManager *ptm;
   ProcessImageMap *pim;
   void *stack_page;
   void *kstack_page;
   FDTable *fd_table;
   unsigned long long state;
   Task *parent;
   Task *next;
   InterruptRegisters regs;
};
typedef struct Task Task;

class TaskManager {
public:
   static void init();
   static void task_switch(InterruptFrame *frame, unsigned long long rax);
   static InterruptRegisters *task_switch(InterruptRegisters *regs);
   static long long fork(InterruptFrame *frame);
   static void fork(InterruptRegisters *regs);
   static void user_exec(char const *path, char const **argv,  char const **envp);
   static void move_stack(void *oldstack, void *newstack);
   static long long getpid();
   static void exit();
   static Task *current_task;
   static Task *ready_queue;
private:
   static long long next_pid;
};