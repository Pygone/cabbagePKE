/*
 * Utility functions for process management.
 *
 * Note: in Lab1, only one process (i.e., our user application) exists. Therefore,
 * PKE OS at this stage will set "current" to the loaded user application, and also
 * switch to the old "current" process after trap handling.
 */

#include "riscv.h"
#include "strap.h"
#include "config.h"
#include "process.h"
#include "elf.h"
#include "string.h"
#include "vmm.h"
#include "pmm.h"
#include "memlayout.h"
#include "spike_interface/spike_utils.h"
// Two functions defined in kernel/usertrap.S
extern char smode_trap_vector[];

extern void return_to_user(trapframe *, uint64 satp);

// current points to the currently running user-mode application.
process *current = NULL;

// points to the first free page in our simple heap. added @lab2_2
uint64 g_ufree_page = USER_FREE_ADDRESS_START;

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
    proc->trapframe->kernel_sp = proc->kstack;     // process's kernel stack
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

void try_alloc_free_block()
{
    if (current->first_free_block == 0)
    {
        uint64 page = (uint64)alloc_page();
        uint64 va = g_ufree_page;
        g_ufree_page += PGSIZE;
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
        last_va = g_ufree_page;
        g_ufree_page += PGSIZE;
        memset((void *)page, 0, PGSIZE);
        user_vm_map(current->pagetable, last_va, PGSIZE, page, prot_to_type(PROT_WRITE | PROT_READ, 1));
        if (need_size > PGSIZE)
            need_size -= PGSIZE;
    } while (need_size > PGSIZE);
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