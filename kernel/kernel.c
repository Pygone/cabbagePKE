/*
 * Supervisor-mode startup codes
 */

#include <spike_interface/atomic.h>


#include "elf.h"
#include "memlayout.h"
#include "pmm.h"
#include "process.h"
#include "ramdev.h"
#include "rfs.h"
#include "riscv.h"
#include "sched.h"
#include "spike_interface/spike_utils.h"
#include "string.h"
#include "util/types.h"
#include "vfs.h"
#include "vmm.h"

//
// trap_sec_start points to the beginning of S-mode trap segment (i.e., the entry point of
// S-mode trap vector). added @lab2_1
//
extern char trap_sec_start[];

//
// turn on paging. added @lab2_1
//
void enable_paging()
{
    // write the pointer to kernel page (table) directory into the CSR of "satp".
    write_csr(satp, MAKE_SATP(g_kernel_pagetable));

    // refresh tlb to invalidate its content.
    flush_tlb();
}

typedef union
{
    uint64 buf[MAX_CMDLINE_ARGS];
    char *argv[MAX_CMDLINE_ARGS];
} arg_buf;

//
// returns the number (should be 1) of string(s) after PKE kernel in command line.
// and store the string(s) in arg_bug_msg.
//
static size_t parse_args(arg_buf *arg_bug_msg)
{
    // HTIFSYS_getmainvars frontend call reads command arguments to (input) *arg_bug_msg
    long r = frontend_syscall(HTIFSYS_getmainvars, (uint64)arg_bug_msg, sizeof(*arg_bug_msg), 0, 0, 0, 0, 0);
    kassert(r == 0);

    size_t pk_argc = arg_bug_msg->buf[0];
    uint64 *pk_argv = &arg_bug_msg->buf[1];

    int arg = 1; // skip the PKE OS kernel string, leave behind only the application name
    for (size_t i = 0; arg + i < pk_argc; i++)
        arg_bug_msg->argv[i] = (char *)(uintptr_t)pk_argv[arg + i];

    // returns the number of strings after PKE kernel in command line
    return pk_argc - arg;
}

static spinlock_t latch_;
static int inited = 0;
static int cnt = 0;
static int has_parsed = 0;
static arg_buf arg_bug_msg;
//
// load the elf, and construct a "process" (with only a trapframe).
// load_bincode_from_host_elf is defined in elf.c
//
process *load_user_program()
{
    int hart_id = read_tp();
    process *proc = alloc_process();
    proc->trapframe->regs.tp = hart_id;
    sprint("User application is loading.\n");
    spinlock_lock(&latch_);
    if (has_parsed == 0)
    {
        has_parsed = 1;
        size_t argc = parse_args(&arg_bug_msg);
        if (!argc)
            panic("You need to specify the application program!\n");
    }
    spinlock_unlock(&latch_);
    load_bincode_from_host_elf(proc, arg_bug_msg.argv[hart_id]);
    proc->first_free_block = 0;
    return proc;
}


//
// s_start: S-mode entry point of riscv-pke OS kernel.
//
int s_start(void)
{
    int hart_id = read_tp();
    sprint("hartid = %d: Enter supervisor mode...\n", hart_id);

    // in the beginning, we use Bare mode (direct) memory mapping as in lab1.
    // but now, we are going to switch to the paging mode @lab2_1.
    // note, the code still works in Bare mode when calling pmm_init() and kern_vm_init().
    write_csr(satp, 0);

    spinlock_lock(&latch_);
    if (!inited)
    {
        inited = 1;
        // init phisical memory manager
        pmm_init();

        // build the kernel page table
        kern_vm_init();

        // now, switch to paging mode by turning on paging (SV39)
        enable_paging();
        // the code now formally works in paging mode, meaning the page table is now in use.
        sprint("kernel page table is on \n");


        // init file system, added @lab4_1
        fs_init();
    }
    spinlock_unlock(&latch_);
    // added @lab3_1
    init_proc_pool();
    vm_alloc_stage[hart_id] = 1;
    sprint("Switch to user mode...\n");
    // the application code (elf) is first loaded into memory, and then put into execution
    // added @lab3_1
    insert_to_ready_queue(load_user_program());
    schedule();

    // we should never reach here.
    return 0;
}
