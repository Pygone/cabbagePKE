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

#include "spike_interface/spike_utils.h"

//Two functions defined in kernel/usertrap.S
extern char smode_trap_vector[];

extern void return_to_user(trapframe *);

// current points to the currently running user-mode application.
process *current = NULL;

//
// switch to a user-mode process
//
void switch_to(process *proc) {
    assert(proc);
    current = proc;

    // write the smode_trap_vector (64-bit func. address) defined in kernel/strap_vector.S
    // to the stvec privilege register, such that trap handler pointed by smode_trap_vector
    // will be triggered when an interrupt occurs in S mode.
    write_csr(stvec, (uint64)smode_trap_vector);

    // set up trapframe values (in process structure) that smode_trap_vector will need when
    // the process next re-enters the kernel.
    proc->trapframe->kernel_sp = proc->kstack; // process's kernel stack
    proc->trapframe->kernel_trap = (uint64) smode_trap_handler;

    // SSTATUS_SPP and SSTATUS_SPIE are defined in kernel/riscv.h
    // set S Previous Privilege mode (the SSTATUS_SPP bit in sstatus register) to User mode.
    unsigned long x = read_csr(sstatus);
    x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
    x |= SSTATUS_SPIE; // enable interrupts in user mode

    // write x back to 'sstatus' register to enable interrupts, and sret destination mode.
    write_csr(sstatus, x);

    // set S Exception Program Counter (sepc register) to the elf entry pc.
    write_csr(sepc, proc->trapframe->epc);

    // return_to_user() is defined in kernel/strap_vector.S. switch to user mode with sret.
    return_to_user(proc->trapframe);
}


void print_error_line(const uint64 mepc) {
    struct stat st;
    for (int i = 0; i < current->line_ind; i++) {
        if (current->line[i].addr > mepc) {
            if (current->line[i].addr <= mepc) continue;
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
            while (idx < st.st_size) {
                if (buf[idx] == '\n' && ++cnt == line - 1) {
                    for (int j = idx + 1; j < st.st_size && buf[j] != '\n'; j++) {
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
