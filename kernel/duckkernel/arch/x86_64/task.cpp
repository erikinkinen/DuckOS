#include <task.h>
#include <memory/heap.h>
#include <memory/pageframe.h>
#include <utils/logger.h>

volatile Task *TaskManager::current_task = nullptr;
volatile Task *TaskManager::ready_queue = nullptr;
long long TaskManager::next_pid = 0;

extern "C" {
    extern __attribute__((noreturn)) void arch_restore_context(volatile Thread *buf);
    extern __attribute__((returns_twice)) int arch_save_context(volatile Thread *buf);
}

void TaskManager::init() {
#if KDEBUG == true
    Logger::info("Initializing multitasking...");
#endif
    asm volatile ("cli");
    current_task = ready_queue = (Task *)Heap::k.alloc(sizeof(Task));
    current_task->id = next_pid++;
    current_task->next = nullptr;
    current_task->fd_table = new FDTable();
    current_task->thread.ptm = PageTableManager::k;
#if KDEBUG == true
    Logger::ok();
#endif
    asm volatile ("sti");
    arch_save_context(&current_task->thread);
}

void TaskManager::task_switch() {
    if (!current_task) return;
    int s = arch_save_context(&current_task->thread);
    if (s == 1) return;
    current_task = current_task->next;
    if (!current_task) current_task = ready_queue;
    current_task->thread.ptm->enable();
    arch_restore_context(&current_task->thread);
}

long long TaskManager::fork() {
    asm volatile ("cli");
    Task *parent = (Task *)current_task;
    Task *new_task = (Task *)Heap::k.alloc(sizeof(Task));
    new_task->id = next_pid++;
    new_task->next = nullptr;
    new_task->fd_table = new FDTable();
    Task *tmp_task = (Task*)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    tmp_task->next = new_task; 
    void *newstack = PageFrameAllocator::alloc();
    move_stack(newstack);
    arch_save_context(&new_task->thread);
    if (current_task == parent) {
        unsigned long long rsp, rbp;
        asm volatile("mov %%rsp, %%rax" : "=a"(rsp));
        asm volatile("mov %%rbp, %%rax" : "=a"(rbp));
        new_task->thread.context.sp = (unsigned long long)newstack + rsp % 4096;
        new_task->thread.context.bp = (unsigned long long)newstack + rbp % 4096;
        asm volatile ("sti");
        return new_task->id;
    }
    return 0;
}

void TaskManager::user_exec(char const *path, char const **argv,  char const **envp) {
    Stat *prg_stat = new Stat();
    long long status = FSManager::stat(path, prg_stat);
    if (status < 0) return;
    ElfHeader *header = (ElfHeader *)Heap::k.alloc(prg_stat->size);
    long long fd = FSManager::open(path, 4);
    long long readsize = FSManager::read(fd, (void *)header, prg_stat->size);
    if (readsize < 64) return;
    if (!(header->magic[0] == 0x7f && header->magic[1] == 'E' && header->magic[2] == 'L' && header->magic[3] == 'F'))
        return;
    if (header->bitversion != 2) return;
    if (header->type != 2) return;
    if (header->inst_set != 0x3e) return;

    PageTableManager *p_ptm = new PageTableManager();

    for (unsigned long long i = 0; i < Memory::size(); i += 4096) {
        p_ptm->map((void *)i, (void *)i, true, false);
    }

    for (unsigned long long i = (unsigned long long)bInfo.fb.base_addr; 
        i < (unsigned long long)bInfo.fb.base_addr + bInfo.fb.size + 0x1000; 
        i += 4096)
    {
        p_ptm->map((void*)i, (void*)i, true, false);
    }

    ElfProgramHeader *phdr = (ElfProgramHeader *)((unsigned long long)header + header->phdr_pos);
    for (unsigned long long i = 0; i < header->phdr_entry_count; i++) {
        if (phdr->type == 1) {
            void *seg = (void *)(((unsigned long long)PageFrameAllocator::alloc(phdr->p_memsz / 4096 + 1) & 0xFFFFFFFFFFFFF000) + 4096);
            if (phdr->p_memsz >= phdr->p_filesz) {
                for (unsigned long long j = 0; j < phdr->p_filesz; j++) {
                    ((char *)seg)[j + phdr->p_vaddr % 4096] = ((char *)header)[phdr->p_offset + j];
                }
                for (unsigned long long j = 0; j < phdr->p_filesz / 4096 + 1; j++) {
                    p_ptm->map((void *)((phdr->p_vaddr & 0xFFFFFFFFFFFFF000) + j * 4096), (void *)((unsigned long long)seg + j * 4096), true, true);
                }
            }
        }
        phdr = (ElfProgramHeader *)((unsigned long long)phdr + header->phdr_entry_size);
    }

    void *stack_page = PageFrameAllocator::alloc();
    p_ptm->map(stack_page, stack_page, true, true);

    current_task->thread.ptm = p_ptm;
    p_ptm->enable();

    void *stack =  (void *)((char*)stack_page + 0xFFF);
    asm volatile (
        "cli\n"
        "pushq $0x23\n"
        "pushq %0\n"
        "pushq $0x202\n"
        "pushq $0x1b\n"
        "pushq %1\n"
        "iretq;"
    : : "m"(stack), "m"(header->entry_pos) : "memory");
}

void TaskManager::move_stack(void *newstack) {
    unsigned long long original_sp;
    asm volatile("mov %%rsp, %%rax" : "=a"(original_sp));
    unsigned char *oldstack = (unsigned char *)(original_sp & 0xFFFFFFFFFFFFF000);
    unsigned long long counter = 0;
    unsigned char tmp;
    asm volatile(
        "start_loop:\n\t"
        "movb (%3, %q1, 1), %0\n\t"
        "movb %0, (%2, %q1, 1)\n\t"
        "addq $1, %1\n\t"
        "cmp $4096, %1\n\t"
        "jne start_loop"
        : "=&r" (tmp), "+r" (counter)
        : "r" (newstack), "r"(oldstack)
        : "cc", "memory"
        );
}

long long TaskManager::getpid() {
    return current_task->id;
}