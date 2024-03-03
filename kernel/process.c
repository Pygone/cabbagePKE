/*
 * Utility functions for process management.
 *
 * Note: in Lab1, only one process (i.e., our user application) exists. Therefore,
 * PKE OS at this stage will set "current" to the loaded user application, and also
 * switch to the old "current" process after trap handling.
 */

#include "process.h"
#include "config.h"
#include "elf.h"
#include "memlayout.h"
#include "pmm.h"
#include "riscv.h"
#include "sched.h"
#include "spike_interface/spike_utils.h"
#include "strap.h"
#include "string.h"
#include "vmm.h"

// Two functions defined in kernel/usertrap.S
extern char smode_trap_vector[];
extern void return_to_user(trapframe *, uint64 satp);

// trap_sec_start points to the beginning of S-mode trap segment (i.e., the entry point
// of S-mode trap vector).
extern char trap_sec_start[];

// process pool. added @lab3_1
process procs[NPROC];

// current points to the currently running user-mode application.
process *current = NULL;

//
// switch to a user-mode process
//
void switch_to(process *proc)
{
    assert(proc);
    current = proc;

    // write the smode_trap_vector (64-bit func. address) defined in kernel/strap_vector.S
    // to the stvec privilege register, such that trap handler pointed by smode_trap_vector
    // will be triggered when an interrupt occurs in S mode.
    write_csr(stvec, (uint64)smode_trap_vector);

    // set up trapframe values (in process structure) that smode_trap_vector will need when
    // the process next re-enters the kernel.
    proc->trapframe->kernel_sp = proc->kstack; // process's kernel stack
    proc->trapframe->kernel_satp = read_csr(satp); // kernel page table
    proc->trapframe->kernel_trap = (uint64)smode_trap_handler;

    // SSTATUS_SPP and SSTATUS_SPIE are defined in kernel/riscv.h
    // set S Previous Privilege mode (the SSTATUS_SPP bit in sstatus register) to User mode.
    unsigned long x = read_csr(sstatus);
    x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
    x |= SSTATUS_SPIE; // enable interrupts in user mode

    // write x back to 'sstatus' register to enable interrupts, and sret destination mode.
    write_csr(sstatus, x);

    // set S Exception Program Counter (sepc register) to the elf entry pc.
    write_csr(sepc, proc->trapframe->epc);

    // make user page table. macro MAKE_SATP is defined in kernel/riscv.h. added @lab2_1
    uint64 user_satp = MAKE_SATP(proc->pagetable);

    // return_to_user() is defined in kernel/strap_vector.S. switch to user mode with sret.
    // note, return_to_user takes two parameters @ and after lab2_1.
    return_to_user(proc->trapframe, user_satp);
}

//
// initialize process pool (the procs[] array). added @lab3_1
//
void init_proc_pool()
{
    memset(procs, 0, sizeof(process) * NPROC);

    for (int i = 0; i < NPROC; ++i)
    {
        procs[i].status = FREE;
        procs[i].pid = i;
    }
}

//
// allocate an empty process, init its vm space. returns the pointer to
// process strcuture. added @lab3_1
//
process *alloc_process()
{
    // locate the first usable process structure
    int i;

    for (i = 0; i < NPROC; i++)
        if (procs[i].status == FREE)
            break;

    if (i >= NPROC)
    {
        panic("cannot find any free process structure.\n");
        return 0;
    }

    // init proc[i]'s vm space
    procs[i].trapframe = (trapframe *)alloc_page(); // trapframe, used to save context
    memset(procs[i].trapframe, 0, sizeof(trapframe));

    // page directory
    procs[i].pagetable = (pagetable_t)alloc_page();
    memset((void *)procs[i].pagetable, 0, PGSIZE);

    procs[i].kstack = (uint64)alloc_page() + PGSIZE; // user kernel stack top
    uint64 user_stack = (uint64)alloc_page(); // phisical address of user stack bottom
    procs[i].trapframe->regs.sp = USER_STACK_TOP; // virtual address of user stack top

    // allocates a page to record memory regions (segments)
    procs[i].mapped_info = (mapped_region *)alloc_page();
    memset(procs[i].mapped_info, 0, PGSIZE);

    // map user stack in userspace
    user_vm_map((pagetable_t)procs[i].pagetable, USER_STACK_TOP - PGSIZE, PGSIZE, user_stack,
                prot_to_type(PROT_WRITE | PROT_READ, 1));
    procs[i].mapped_info[STACK_SEGMENT].va = USER_STACK_TOP - PGSIZE;
    procs[i].mapped_info[STACK_SEGMENT].npages = 1;
    procs[i].mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;

    // map trapframe in user space (direct mapping as in kernel space).
    user_vm_map((pagetable_t)procs[i].pagetable, (uint64)procs[i].trapframe, PGSIZE, (uint64)procs[i].trapframe,
                prot_to_type(PROT_WRITE | PROT_READ, 0));
    procs[i].mapped_info[CONTEXT_SEGMENT].va = (uint64)procs[i].trapframe;
    procs[i].mapped_info[CONTEXT_SEGMENT].npages = 1;
    procs[i].mapped_info[CONTEXT_SEGMENT].seg_type = CONTEXT_SEGMENT;

    // map S-mode trap vector section in user space (direct mapping as in kernel space)
    // we assume that the size of usertrap.S is smaller than a page.
    user_vm_map((pagetable_t)procs[i].pagetable, (uint64)trap_sec_start, PGSIZE, (uint64)trap_sec_start,
                prot_to_type(PROT_READ | PROT_EXEC, 0));
    procs[i].mapped_info[SYSTEM_SEGMENT].va = (uint64)trap_sec_start;
    procs[i].mapped_info[SYSTEM_SEGMENT].npages = 1;
    procs[i].mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;


    // initialize the process's heap manager
    procs[i].user_heap.heap_top = USER_FREE_ADDRESS_START;
    procs[i].user_heap.heap_bottom = USER_FREE_ADDRESS_START;
    procs[i].user_heap.free_pages_count = 0;

    // map user heap in userspace
    procs[i].mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
    procs[i].mapped_info[HEAP_SEGMENT].npages = 0; // no pages are mapped to heap yet.
    procs[i].mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;

    procs[i].total_mapped_region = 4;

    // initialize files_struct
    procs[i].pfiles = init_proc_file_management();

    // return after initialization.
    return &procs[i];
}
static void wakeup(struct process_t *pProcess)
{
    if (pProcess == NULL)
    {
        return;
    }
    if (pProcess->status == BLOCKED)
    {
        pProcess->status = READY;
        insert_to_ready_queue(pProcess);
    }
}
//
// reclaim a process. added @lab3_1
//
int free_process(process *proc)
{
    // we set the status to ZOMBIE, but cannot destruct its vm space immediately.
    // since proc can be current process, and its user kernel stack is currently in use!
    // but for proxy kernel, it (memory leaking) may NOT be a really serious issue,
    // as it is different from regular OS, which needs to run 7x24.
    proc->status = ZOMBIE;
    wakeup(proc->parent);
    return 0;
}

//
// implements fork syscal in kernel. added @lab3_1
// basic idea here is to first allocate an empty process (child), then duplicate the
// context and data segments of parent process to the child, and lastly, map other
// segments (code, system) of the parent to child. the stack segment remains unchanged
// for the child.
//
int do_fork(process *parent)
{
    process *child = alloc_process();

    for (int i = 0; i < parent->total_mapped_region; i++)
    {
        // browse parent's vm space, and copy its trapframe and data segments,
        // map its code segment.
        switch (parent->mapped_info[i].seg_type)
        {
        case CONTEXT_SEGMENT:
            *child->trapframe = *parent->trapframe;
            break;
        case STACK_SEGMENT:
            memcpy((void *)lookup_pa(child->pagetable, child->mapped_info[STACK_SEGMENT].va),
                   (void *)lookup_pa(parent->pagetable, parent->mapped_info[i].va), PGSIZE);
            break;
        case HEAP_SEGMENT:
            // build a same heap for child process.

            // convert free_pages_address into a filter to skip reclaimed blocks in the heap
            // when mapping the heap blocks
            int free_block_filter[MAX_HEAP_PAGES];
            memset(free_block_filter, 0, MAX_HEAP_PAGES);
            uint64 heap_bottom = parent->user_heap.heap_bottom;
            for (int i = 0; i < parent->user_heap.free_pages_count; i++)
            {
                int index = (parent->user_heap.free_pages_address[i] - heap_bottom) / PGSIZE;
                free_block_filter[index] = 1;
            }

            // copy and map the heap blocks
            for (uint64 heap_block = current->user_heap.heap_bottom; heap_block < current->user_heap.heap_top;
                 heap_block += PGSIZE)
            {
                if (free_block_filter[(heap_block - heap_bottom) / PGSIZE]) // skip free blocks
                    continue;

                void *child_pa = alloc_page();
                memcpy(child_pa, (void *)lookup_pa(parent->pagetable, heap_block), PGSIZE);
                user_vm_map((pagetable_t)child->pagetable, heap_block, PGSIZE, (uint64)child_pa,
                            prot_to_type(PROT_WRITE | PROT_READ, 1));
            }

            child->mapped_info[HEAP_SEGMENT].npages = parent->mapped_info[HEAP_SEGMENT].npages;
            // copy the heap manager from parent to child
            memcpy((void *)&child->user_heap, (void *)&parent->user_heap, sizeof(parent->user_heap));
            break;
        case CODE_SEGMENT:
            // TODO (lab3_1): implment the mapping of child code segment to parent's
            // code segment.
            // hint: the virtual address mapping of code segment is tracked in mapped_info
            // page of parent's process structure. use the information in mapped_info to
            // retrieve the virtual to physical mapping of code segment.
            // after having the mapping information, just map the corresponding virtual
            // address region of child to the physical pages that actually store the code
            // segment of parent process.
            // DO NOT COPY THE PHYSICAL PAGES, JUST MAP THEM.
            user_vm_map((pagetable_t)child->pagetable, parent->mapped_info[i].va,
                        parent->mapped_info[i].npages * PGSIZE, lookup_pa(parent->pagetable, parent->mapped_info[i].va),
                        prot_to_type(PROT_READ | PROT_EXEC, 1));

            // after mapping, register the vm region (do not delete codes below!)
            child->mapped_info[child->total_mapped_region].va = parent->mapped_info[i].va;
            child->mapped_info[child->total_mapped_region].npages = parent->mapped_info[i].npages;
            child->mapped_info[child->total_mapped_region].seg_type = CODE_SEGMENT;
            child->total_mapped_region++;
            break;
        case DATA_SEGMENT:
            for (int j = 0; j < parent->mapped_info[i].npages; j++)
            {
                uint64 addr = lookup_pa(parent->pagetable, parent->mapped_info[i].va + j * PGSIZE);
                char *newaddr = alloc_page();
                memcpy(newaddr, (void *)addr, PGSIZE);
                map_pages(child->pagetable, parent->mapped_info[i].va + j * PGSIZE, PGSIZE, (uint64)newaddr,
                          prot_to_type(PROT_WRITE | PROT_READ, 1));
            }

            // after mapping, register the vm region (do not delete codes below!)
            child->mapped_info[child->total_mapped_region].va = parent->mapped_info[i].va;
            child->mapped_info[child->total_mapped_region].npages = parent->mapped_info[i].npages;
            child->mapped_info[child->total_mapped_region].seg_type = DATA_SEGMENT;
            child->total_mapped_region++;
            break;
        }
    }
    child->pfiles->nfiles = parent->pfiles->nfiles;
    child->pfiles->cwd = parent->pfiles->cwd;
    for (int i = 0; i < MAX_FILES; i++)
    {
        child->pfiles->opened_files[i] = parent->pfiles->opened_files[i];
        if (child->pfiles->opened_files[i].f_dentry != NULL)
            child->pfiles->opened_files[i].f_dentry->d_ref++;
    }

    child->status = READY;
    child->trapframe->regs.a0 = 0;
    child->parent = parent;
    insert_to_ready_queue(child);

    return child->pid;
}

int do_wait(int pid)
{
    // 当pid为-1时，父进程等待任意一个子进程退出即返回子进程的pid；
    // 当pid大于0时，父进程等待进程号为pid的子进程退出即返回子进程的pid；
    // 如果pid不合法或pid大于0且pid对应的进程不是当前进程的子进程，返回-1。
    if (pid == -1)
    {
        int flag = 0;
        for (int i = 0; i < NPROC; i++)
        {
            if (procs[i].parent == current && procs[i].status == ZOMBIE)
            {
                procs[i].status = FREE;
                return procs[i].pid;
            }
            else if (procs[i].parent == current && procs[i].status != ZOMBIE)
            {
                flag = 1;
            }
        }
        if (flag == 0)
        {
            return -1;
        }
        else
        {
            current->status = BLOCKED;
            schedule();
            return do_wait(-1);
        }
    }
    else if (pid > 0 && pid < NPROC)
    {
        if (procs[pid].parent == current && procs[pid].status == ZOMBIE)
        {
            procs[pid].status = FREE;
            return pid;
        }
        else if (procs[pid].parent == current && procs[pid].status != ZOMBIE)
        {
            current->status = BLOCKED;
            schedule();
            return do_wait(pid);
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

void proc_clean_pagetable(process *p)
{

    int total_mapped_region = p->total_mapped_region;
    for (int i = 0; i < total_mapped_region; i++)
    {
        if (p->mapped_info[i].seg_type == HEAP_SEGMENT)
        { // 从后往前分配, 故要从后往前释放
            uint64 top = p->user_heap.heap_top - PGSIZE;
            for (int j = 0; j < p->mapped_info[i].npages; j++)
            {
                uint64 pa = lookup_pa(p->pagetable, top);
                top -= PGSIZE;
                free_page((void *)pa);
            }
        }
        else
        {
            pte_t *pte;
            uint64 va = p->mapped_info[i].va;
            int64 size = p->mapped_info[i].npages;
            while (size > 0)
            {
                pte = page_walk(p->pagetable, va, 0);
                if (((*pte & PTE_W) && (*pte & PTE_V)))
                {
                    uint64 pa = PTE2PA(*pte);
                    free_page((void *)pa);
                    *pte &= ~PTE_V;
                }
                va += PGSIZE;
                size -= 1;
            }
        }
    }
}

void exec_clean(process *p)
{
    // 释放原先内存
    proc_clean_pagetable(p);

    // init proc[i]'s vm space
    p->trapframe = (trapframe *)alloc_page(); // trapframe, used to save context
    memset(p->trapframe, 0, sizeof(trapframe));

    // page directory
    memset((void *)p->pagetable, 0, PGSIZE);

    uint64 user_stack = (uint64)alloc_page(); // phisical address of user stack bottom
    p->trapframe->regs.sp = USER_STACK_TOP; // virtual address of user stack top

    // allocates a page to record memory regions (segments)
    memset(p->mapped_info, 0, PGSIZE);

    // map user stack in userspace
    user_vm_map((pagetable_t)p->pagetable, USER_STACK_TOP - PGSIZE, PGSIZE, user_stack,
                prot_to_type(PROT_WRITE | PROT_READ, 1));
    p->mapped_info[STACK_SEGMENT].va = USER_STACK_TOP - PGSIZE;
    p->mapped_info[STACK_SEGMENT].npages = 1;
    p->mapped_info[STACK_SEGMENT].seg_type = STACK_SEGMENT;

    // map trapframe in user space (direct mapping as in kernel space).
    user_vm_map((pagetable_t)p->pagetable, (uint64)p->trapframe, PGSIZE, // trapframe的物理地址等于虚拟地址
                (uint64)p->trapframe, prot_to_type(PROT_WRITE | PROT_READ, 0));
    p->mapped_info[CONTEXT_SEGMENT].va = (uint64)p->trapframe;
    p->mapped_info[CONTEXT_SEGMENT].npages = 1;
    p->mapped_info[CONTEXT_SEGMENT].seg_type = CONTEXT_SEGMENT;

    // map S-mode trap vector section in user space (direct mapping as in kernel space)
    // we assume that the size of usertrap.S is smaller than a page.
    user_vm_map((pagetable_t)p->pagetable, (uint64)trap_sec_start, PGSIZE, (uint64)trap_sec_start,
                prot_to_type(PROT_READ | PROT_EXEC, 0));
    p->mapped_info[SYSTEM_SEGMENT].va = (uint64)trap_sec_start;
    p->mapped_info[SYSTEM_SEGMENT].npages = 1;
    p->mapped_info[SYSTEM_SEGMENT].seg_type = SYSTEM_SEGMENT;

    // initialize the process's heap manager
    p->user_heap.heap_top = USER_FREE_ADDRESS_START;
    p->user_heap.heap_bottom = USER_FREE_ADDRESS_START;
    p->user_heap.free_pages_count = 0;

    // map user heap in userspace
    p->mapped_info[HEAP_SEGMENT].va = USER_FREE_ADDRESS_START;
    p->mapped_info[HEAP_SEGMENT].npages = 0; // no pages are mapped to heap yet.
    p->mapped_info[HEAP_SEGMENT].seg_type = HEAP_SEGMENT;

    p->total_mapped_region = 4;
}

void print_error_line(const uint64 mepc)
{
    struct stat st;
    for (int i = 0; i < current->line_ind; i++)
    {
        if (current->line[i].addr > mepc)
        {
            if (current->line[i].addr <= mepc)
                continue;
            char *dir = current->dir[current->file[current->line[i - 1].file].dir];
            char *file = current->file[current->line[i - 1].file].file;
            const int line = current->line[i - 1].line;
            sprint("Runtime error at %s/%s:%d\n", dir, file, line);
            char filename[256];
            char *p = filename;
            strcpy(p, current->dir[current->file[current->line[i - 1].file].dir]);
            p += strlen(p);
            p[0] = '/', p[1] = 0;
            p += 1;
            strcpy(p, current->file[current->line[i - 1].file].file);
            spike_file_t *fp = spike_file_open(filename, O_RDONLY, 0);
            spike_file_stat(fp, &st);
            char buf[st.st_size];
            spike_file_read(fp, buf, st.st_size);
            spike_file_close(fp);
            int idx = 0;
            int cnt = 0;
            while (idx < st.st_size)
            {
                if (buf[idx] == '\n' && ++cnt == line - 1)
                {
                    for (int j = idx + 1; j < st.st_size && buf[j] != '\n'; j++)
                    {
                        sprint("%c", buf[j]);
                    }
                    break;
                }
                idx++;
            }
            sprint("\n");
            break;
        }
    }
}

void try_alloc_free_block()
{
    if (current->first_free_block == 0)
    {
        uint64 page = (uint64)alloc_page();
        uint64 va = current->user_heap.heap_top;
        current->user_heap.heap_top += PGSIZE;
        current->mapped_info[HEAP_SEGMENT].npages++;
        memset((void *)page, 0, PGSIZE);
        user_vm_map(current->pagetable, va, PGSIZE, page, prot_to_type(PROT_WRITE | PROT_READ, 1));
        block_head *head = (block_head *)(user_va_to_pa(current->pagetable, (void *)va));
        head->capacity = PGSIZE;
        head->next = 0;
        current->first_free_block = va;
    }
}

void *alloc_from_new_block(uint64 n)
{
    uint64 need_size = n + sizeof(block_head);
    uint64 need_pages = (need_size + PGSIZE - 1) / PGSIZE; // 向上取整
    // 先利用va最大的block, 然后再继续分配block, 保证内存分配的连续性
    uint64 cur = current->first_free_block;
    uint64 max_va = 0, pre = 0, max_pre = 0;
    while (cur != 0)
    {
        block_head *head = (block_head *)(user_va_to_pa(current->pagetable, (void *)cur));
        if (cur > max_va)
        {
            max_pre = pre;
            max_va = cur;
        }
        pre = cur;
        cur = head->next;
    }
    block_head *head = (block_head *)(user_va_to_pa(current->pagetable, (void *)max_va));

    uint64 capacity = head->capacity;
    uint64 next = head->next;
    head->next = 0;
    head->capacity = need_size;
    need_size -= capacity;
    uint64 last_va = 0;
    do
    {
        uint64 page = (uint64)alloc_page();
        last_va = current->user_heap.heap_top;
        current->user_heap.heap_top += PGSIZE;
        current->mapped_info[HEAP_SEGMENT].npages++;
        memset((void *)page, 0, PGSIZE);
        user_vm_map(current->pagetable, last_va, PGSIZE, page, prot_to_type(PROT_WRITE | PROT_READ, 1));
        if (need_size > PGSIZE)
            need_size -= PGSIZE;
    }
    while (need_size > PGSIZE);
    if (need_size != 0)
    { // 最后一个page没用完, 分配新的head, 给剩余空间
        uint64 ALIGN = need_size % sizeof(block_head);
        if (ALIGN != 0)
        {
            need_size += sizeof(block_head) - ALIGN;
        }
        block_head *last_head = (block_head *)(user_va_to_pa(current->pagetable, (void *)(last_va + need_size)));
        last_head->capacity = PGSIZE - need_size;
        last_head->next = next;
        // get head pre
        block_head *max_pre_head;
        if (max_pre != 0)
        {
            max_pre_head = (block_head *)(user_va_to_pa(current->pagetable, (void *)max_pre));
            max_pre_head->next = last_va + need_size;
        }
        else
            current->first_free_block = last_va + need_size;
    }
    return (void *)(max_va + sizeof(block_head));
}

void *alloc_from_free_block(uint64 n)
{
    uint64 need_size = n + sizeof(block_head);
    uint64 need_pages = (need_size + PGSIZE - 1) / PGSIZE; // 向上取整
    uint64 cur = current->first_free_block;
    uint64 pre = 0;
    while (cur != 0)
    {
        block_head *head = (block_head *)(user_va_to_pa(current->pagetable, (void *)cur));
        uint64 capacity = head->capacity;
        if (capacity >= need_size)
        { // 剩余block 足够填充
            if (capacity == need_size)
            { // 恰好填充完, 更新free_block
                if (pre == 0)
                { // 说明是第一个block
                    current->first_free_block = head->next;
                }
                else
                {
                    block_head *pre_head = (block_head *)(user_va_to_pa(current->pagetable, (void *)pre));
                    pre_head->next = head->next;
                }
                return (void *)(cur + sizeof(block_head));
            }
            else
            { // 剩余block 大于需要的block, 更新first_free_block
                uint64 ALIGN = need_size % sizeof(block_head);
                if (ALIGN != 0)
                {
                    need_size += sizeof(block_head) - ALIGN;
                }
                head->capacity = need_size;
                uint64 va = cur + need_size;
                block_head *new_head = (block_head *)(user_va_to_pa(current->pagetable, (void *)va));
                new_head->capacity = capacity - need_size;
                new_head->next = head->next;
                if (pre == 0)
                { // 说明是第一个block
                    current->first_free_block = va;
                }
                else
                {
                    block_head *pre_head = (block_head *)(user_va_to_pa(current->pagetable, (void *)pre));
                    pre_head->next = va;
                }
                return (void *)(cur + sizeof(block_head));
            }
        }
        else
        {
            pre = cur;
            cur = head->next;
        }
    }
    // 所有block 都不满足
    return alloc_from_new_block(n);
}

void *vm_malloc(uint64 n)
{
    void *va = NULL;
    try_alloc_free_block();
    va = alloc_from_free_block(n);
    return (void *)va;
}

void vm_free(uint64 va)
{
    block_head *head = (block_head *)(user_va_to_pa(current->pagetable, (void *)(va - sizeof(block_head))));
    head->next = current->first_free_block;
    current->first_free_block = va - sizeof(block_head);
}