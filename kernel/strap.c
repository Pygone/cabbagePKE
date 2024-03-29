/*
 * Utility functions for trap handling in Supervisor mode.
 */

#include "strap.h"

#include "memlayout.h"
#include "pmm.h"
#include "process.h"
#include "riscv.h"
#include "sched.h"
#include "syscall.h"
#include "util/functions.h"
#include "vmm.h"

#include "spike_interface/spike_utils.h"

//
// handling the syscalls. will call do_syscall() defined in kernel/syscall.c
//
static void handle_syscall(trapframe *tf)
{
    // tf->epc points to the address that our computer will jump to after the trap handling.
    // for a syscall, we should return to the NEXT instruction after its handling.
    // in RV64G, each instruction occupies exactly 32 bits (i.e., 4 Bytes)
    tf->epc += 4;

    // TODO (lab1_1): remove the panic call below, and call do_syscall (defined in
    // kernel/syscall.c) to conduct real operations of the kernel side for a syscall.
    // IMPORTANT: return value should be returned to user app, or else, you will encounter
    // problems in later experiments!
    tf->regs.a0 = do_syscall(tf->regs.a0, tf->regs.a1, tf->regs.a2, tf->regs.a3, tf->regs.a4, tf->regs.a5, tf->regs.a6,
                             tf->regs.a7);
}

//
// global variable that store the recorded "ticks". added @lab1_3
static uint64 g_ticks[NCPU] = {0};
//
// added @lab1_3
//
void handle_mtimer_trap()
{
    int hart_id = read_tp();
    sprint("Ticks %d\n", g_ticks[hart_id]);
    // TODO (lab1_3): increase g_ticks to record this "tick", and then clear the "SIP"
    // field in sip register.
    // hint: use write_csr to disable the SIP_SSIP bit in sip.
    g_ticks[hart_id]++;
    write_csr(sip, read_csr(sip) & ~SIP_SSIP);
}

//
// the page fault handler. added @lab2_3. parameters:
// sepc: the pc when fault happens;
// stval: the virtual address that causes pagefault when being accessed.
//
void handle_user_page_fault(uint64 mcause, uint64 sepc, uint64 stval)
{
    sprint("handle_page_fault: %lx\n", stval);
    int hart_id = read_tp();
    switch (mcause)
    {
    case CAUSE_STORE_PAGE_FAULT:
        // TODO (lab2_3): implement the operations that solve the page fault to
        // dynamically increase application stack.
        // hint: first allocate a new physical page, and then, maps the new page to the
        // virtual address that causes the page fault.
        {
            // Allocate a new physical page
            if (stval > current[hart_id]->trapframe->regs.sp && stval < USER_STACK_TOP) // stack grows from high to low
            {
                void *pa = alloc_page();
                if (pa == NULL)
                {
                    panic("alloc_page failed.\n");
                }
                pte_t *pte = page_walk(current[hart_id]->pagetable, stval, 1);
                *pte = PA2PTE(pa) | PTE_V | prot_to_type(PROT_WRITE | PROT_READ, 1);
            }
            else
            {
                pte_t *pte = page_walk(current[hart_id]->pagetable, stval, 0);
                if ((RSW((*pte))) == 1)
                {
                    const uint64 origin_pa = PTE2PA(*pte);
                    free_page((void *)origin_pa); // 尝试释放掉原来的内存
                    void *pa = alloc_page();
                    if (pa == NULL)
                    {
                        panic("alloc_page failed.\n");
                    }
                    *pte = PA2PTE(pa) | PTE_V | prot_to_type(PROT_WRITE | PROT_READ, 1);
                }
                else
                {
                    panic("this address is not available!");
                }
            }
            break;
        }
    default:
        sprint("unknown page fault.\n");
        break;
    }
}

//
// implements round-robin scheduling. added @lab3_3
//
void rrsched()
{
    int hart_id = read_tp();
    // TODO (lab3_3): implements round-robin scheduling.
    // hint: increase the tick_count member of current process by one, if it is bigger than
    // TIME_SLICE_LEN (means it has consumed its time slice), change its status into READY,
    // place it in the rear of ready queue, and finally schedule next process to run.
    current[hart_id]->tick_count++;
    if (current[hart_id]->tick_count >= TIME_SLICE_LEN)
    {
        current[hart_id]->tick_count = 0;
        current[hart_id]->status = READY;
        insert_to_ready_queue(current[hart_id]);
        schedule();
    }
}

//
// kernel/smode_trap.S will pass control to smode_trap_handler, when a trap happens
// in S-mode.
//
void smode_trap_handler(void)
{
    // make sure we are in User mode before entering the trap handling.
    // we will consider other previous case in lab1_3 (interrupt).
    if ((read_csr(sstatus) & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");
    int hart_id = read_tp();
    assert(current[hart_id]);
    // save user process counter.
    current[hart_id]->trapframe->epc = read_csr(sepc);

    // if the cause of trap is syscall from user application.
    // read_csr() and CAUSE_USER_ECALL are macros defined in kernel/riscv.h
    uint64 cause = read_csr(scause);

    // use switch-case instead of if-else, as there are many cases since lab2_3.
    switch (cause)
    {
    case CAUSE_USER_ECALL:
        handle_syscall(current[hart_id]->trapframe);
        break;
    case CAUSE_MTIMER_S_TRAP:
        handle_mtimer_trap();
        // invoke round-robin scheduler. added @lab3_3
        rrsched();
        break;
    case CAUSE_STORE_PAGE_FAULT:
    case CAUSE_LOAD_PAGE_FAULT:
        // the address of missing page is stored in stval
        // call handle_user_page_fault to process page faults
        handle_user_page_fault(cause, read_csr(sepc), read_csr(stval));
        break;
    default:
        sprint("smode_trap_handler(): unexpected scause %p\n", read_csr(scause));
        sprint("            sepc=%p stval=%p\n", read_csr(sepc), read_csr(stval));
        panic("unexpected exception happened.\n");
        break;
    }

    // continue (come back to) the execution of current process.
    switch_to(current[hart_id]);
}
