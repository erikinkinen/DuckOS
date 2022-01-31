#include <task.h>
#include <memory/heap.h>
#include <memory/pageframe.h>
#include <memory/gdt.h>
#include <utils/logger.h>

Task *TaskManager::current_task = nullptr;
Task *TaskManager::ready_queue = nullptr;
long long TaskManager::next_pid = 0;

void TaskManager::init() {
    LOG_DEBUG("Initializing multitasking...");
    current_task = ready_queue = (Task *)Heap::k.alloc(sizeof(Task));
    current_task->id = next_pid++;
    current_task->next = nullptr;
    current_task->fd_table = new FDTable();
    current_task->ptm = PageTableManager::k;
    LOG_DEBUG_OK();
}

InterruptRegisters *TaskManager::task_switch(InterruptRegisters *regs) {
    if (!current_task) return regs;
    PageTableManager::k->enable();

    current_task->regs = *regs;
    current_task = current_task->next;
    if (!current_task)
        current_task = ready_queue;
    regs = &(current_task->regs);

    current_task->ptm->enable();
    return regs;
}

void TaskManager::fork(InterruptRegisters *regs) {
    PageTableManager::k->enable();
    Task *parent = (Task *)current_task;
    Task *new_task = (Task *)Heap::k.alloc(sizeof(Task));
    new_task->id = next_pid++;
    new_task->next = nullptr;
    new_task->fd_table = current_task->fd_table;

    PageTableManager *p_ptm = new PageTableManager();
    ProcessImageMap *p_pim_f = 0, *p_pim_l = 0;

    for (unsigned long long i = 0; i < Memory::size(); i += 4096) {
        p_ptm->map((void *)i, (void *)i, true, false);
    }

    for (unsigned long long i = (unsigned long long)bInfo.fb.base_addr; 
        i < (unsigned long long)bInfo.fb.base_addr + bInfo.fb.size + 0x1000; 
        i += 4096)
    {
        p_ptm->map((void*)i, (void*)i, true, false);
    }

    for (ProcessImageMap *c_pim = parent->pim; c_pim; c_pim = c_pim->next) {
        void *seg = PageFrameAllocator::alloc(c_pim->size);
        for (unsigned long long j = 0; j < c_pim->size * 4096; j++) {
            ((char *)seg)[j] = ((char *)c_pim->paddr)[j];
        }
        for (unsigned long long j = 0; j < c_pim->size; j++) {
            p_ptm->map((void*)((unsigned long long)c_pim->vaddr + j * 4096), (void*)((unsigned long long)seg + j * 4096), true, true);
        }
        ProcessImageMap *p_pim = new ProcessImageMap();
        p_pim->vaddr = c_pim->vaddr;
        p_pim->paddr = seg;
        p_pim->size = c_pim->size;
        p_pim->next = 0;
        if (p_pim_l) p_pim_l->next = p_pim;
        if (!p_pim_f) p_pim_f = p_pim;
        p_pim_l = p_pim;
    }

    void *newstack_u = PageFrameAllocator::alloc();
    void *oldstack_u = parent->stack_page;
    move_stack(oldstack_u, newstack_u);
    p_ptm->map((void*)0xFFFFFFFFFFFFF000, newstack_u, true, true);
    
    new_task->ptm = p_ptm;
    new_task->pim = p_pim_f;

    new_task->regs = *regs;
    new_task->regs.rax = 0;
    regs->rax = new_task->id;
        
    Task *tmp_task = (Task*)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    tmp_task->next = new_task;
    parent->ptm->enable();
}

void TaskManager::user_exec(char const *path, char const **argv,  char const **envp) {
    asm volatile ("cli");
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
    ProcessImageMap *p_pim_f = 0, *p_pim_l = 0;

    for (unsigned long long i = 0; i < Memory::size(); i += 4096) {
        p_ptm->map((void *)i, (void *)i, true, false);
    }

    for (unsigned long long i = (unsigned long long)bInfo.fb.base_addr; 
        i < (unsigned long long)bInfo.fb.base_addr + bInfo.fb.size + 0x1000; 
        i += 4096)
    {
        p_ptm->map((void*)i, (void*)i, true, true);
    }

    ElfProgramHeader *phdr = (ElfProgramHeader *)((unsigned long long)header + header->phdr_pos);
    for (unsigned long long i = 0; i < header->phdr_entry_count; i++) {
        if (phdr->type == 1) {
            void *seg = (void *)(((unsigned long long)PageFrameAllocator::alloc(phdr->p_memsz / 4096 + 1) & 0xFFFFFFFFFFFFF000) + 4096);
            if (phdr->p_memsz >= phdr->p_filesz) {
                for (unsigned long long j = 0; j < phdr->p_filesz; j++) {
                    ((char *)seg)[j + phdr->p_vaddr % 4096] = ((char *)header)[phdr->p_offset + j];
                }
                for (unsigned long long j = 0; j < phdr->p_memsz / 4096 + 1; j++) {
                    p_ptm->map((void *)((phdr->p_vaddr & 0xFFFFFFFFFFFFF000) + j * 4096), (void *)((unsigned long long)seg + j * 4096), true, true);
                }
                ProcessImageMap *p_pim = new ProcessImageMap();
                p_pim->vaddr = (void *)((phdr->p_vaddr & 0xFFFFFFFFFFFFF000));
                p_pim->paddr = (void *)((unsigned long long)seg);
                p_pim->size = phdr->p_memsz / 4096 + 1;
                p_pim->next = 0;
                if (p_pim_l) p_pim_l->next = p_pim;
                if (!p_pim_f) p_pim_f = p_pim;
                p_pim_l = p_pim;
            }
        }
        phdr = (ElfProgramHeader *)((unsigned long long)phdr + header->phdr_entry_size);
    }

    void *stack_page = PageFrameAllocator::alloc();
    p_ptm->map((void*)0xFFFFFFFFFFFFF000, stack_page, true, true);

    current_task->stack_page = stack_page;
    current_task->ptm = p_ptm;
    current_task->pim = p_pim_f;
    p_ptm->enable();

    asm volatile (
        "pushq $0x23;\n"
        "pushq $0xFFFFFFFFFFFFFFFF;\n"
        "pushq $0x202;\n"
        "pushq $0x1b;\n"
        "pushq %0;\n"
        "iretq;\n"
        :: "m"(header->entry_pos)
    );
}

void TaskManager::move_stack(void *oldstack, void *newstack) {
    for (int i = 0; i < 4096; i++) {
        ((char *)newstack)[i] = ((char *)oldstack)[i];
    }
}

long long TaskManager::getpid() {
    return current_task->id;
}