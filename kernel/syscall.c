/*
 * contains the implementation of all syscalls.
 */

#include <errno.h>
#include <stdint.h>

#include "pmm.h"
#include "proc_file.h"
#include "process.h"
#include "sched.h"
#include "string.h"
#include "syscall.h"

#include "elf.h"
#include "util/functions.h"
#include "util/types.h"
#include "vmm.h"

#include "spike_interface/spike_utils.h"
void func_name_printer(uint64 ret_addr);
//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char *buf, size_t n)
{
    // buf is now an address in user space of the given app's user stack,
    // so we have to transfer it into phisical address (kernel is running in direct mapping).
    int hart_id = read_tp();
    const char *pa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)buf);
    sprint(pa);
    return 0;
}
ssize_t sys_user_printpa(uint64 va)
{
    int hart_id = read_tp();
    uint64 pa = (uint64)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)va);
    sprint("%lx\n", pa);
    return 0;
}
ssize_t sys_user_gets(char *buf, size_t n)
{
    // buf is now an address in user space of the given app's user stack,
    // so we have to transfer it into phisical address (kernel is running in direct mapping).

    int hart_id = read_tp();
    assert(current[hart_id]);
    char *pa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)buf);
    return spike_file_read(stdin, pa, n);
}
//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code)
{
    // reclaim the current process, and reschedule. added @lab3_1
    free_process(current[read_tp()]);
    schedule();
    return 0;
}

//
// maybe, the simplest implementation of malloc in the world ... added @lab2_2
//
uint64 sys_user_allocate_page()
{
    int hart_id = read_tp();
    void *pa = alloc_page();
    uint64 va;
    // if there are previously reclaimed pages, use them first (this does not change the
    // size of the heap)
    if (current[hart_id]->user_heap.free_pages_count > 0)
    {
        va = current[hart_id]->user_heap.free_pages_address[--current[hart_id]->user_heap.free_pages_count];
        assert(va < current[hart_id]->user_heap.heap_top);
    }
    else
    {
        // otherwise, allocate a new page (this increases the size of the heap by one page)
        va = current[hart_id]->user_heap.heap_top;
        current[hart_id]->user_heap.heap_top += PGSIZE;

        current[hart_id]->mapped_info[HEAP_SEGMENT].npages++;
    }
    user_vm_map((pagetable_t)current[hart_id]->pagetable, va, PGSIZE, (uint64)pa,
                prot_to_type(PROT_WRITE | PROT_READ, 1));

    return va;
}

//
// reclaim a page, indicated by "va". added @lab2_2
//
uint64 sys_user_free_page(uint64 va)
{
    int hart_id = read_tp();
    user_vm_unmap((pagetable_t)current[hart_id]->pagetable, va, PGSIZE, 1);
    // add the reclaimed page to the free page list
    current[hart_id]->user_heap.free_pages_address[current[hart_id]->user_heap.free_pages_count++] = va;
    return 0;
}

//
// kerenl entry point of naive_fork
//
ssize_t sys_user_fork() { return do_fork(current[read_tp()]); }

//
// kerenl entry point of yield. added @lab3_2
//
ssize_t sys_user_yield()
{
    // TODO (lab3_2): implment the syscall of yield.
    // hint: the functionality of yield is to give up the processor. therefore,
    // we should set the status of currently running process to READY, insert it in
    // the rear of ready queue, and finally, schedule a READY process to run.
    int hart_id = read_tp();
    current[hart_id]->status = READY;
    insert_to_ready_queue(current[hart_id]);
    schedule();

    return 0;
}

//
// open file
//
ssize_t sys_user_open(char *pathva, int flags)
{
    int hart_id = read_tp();
    char *pathpa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), pathva);
    return do_open(pathpa, flags);
}

//
// read file
//
ssize_t sys_user_read(int fd, char *bufva, uint64 count)
{
    int hart_id = read_tp();
    int i = 0;
    while (i < count)
    { // count can be greater than page size
        uint64 addr = (uint64)bufva + i;
        uint64 pa = lookup_pa((pagetable_t)current[hart_id]->pagetable, addr);
        uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
        uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
        uint64 r = do_read(fd, (char *)pa + off, len);
        i += r;
        if (r < len)
            return i;
    }
    return count;
}

//
// write file
//
ssize_t sys_user_write(int fd, char *bufva, uint64 count)
{
    int hart_id = read_tp();
    int i = 0;
    while (i < count)
    { // count can be greater than page size
        uint64 addr = (uint64)bufva + i;
        uint64 pa = lookup_pa((pagetable_t)current[hart_id]->pagetable, addr);
        uint64 off = addr - ROUNDDOWN(addr, PGSIZE);
        uint64 len = count - i < PGSIZE - off ? count - i : PGSIZE - off;
        uint64 r = do_write(fd, (char *)pa + off, len);
        i += r;
        if (r < len)
            return i;
    }
    return count;
}

//
// lseek file
//
ssize_t sys_user_lseek(int fd, int offset, int whence) { return do_lseek(fd, offset, whence); }

//
// read vinode
//
ssize_t sys_user_stat(int fd, struct istat *istat)
{
    int hart_id = read_tp();
    struct istat *pistat = (struct istat *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), istat);
    return do_stat(fd, pistat);
}

//
// read disk inode
//
ssize_t sys_user_disk_stat(int fd, struct istat *istat)
{
    int hart_id = read_tp();
    struct istat *pistat = (struct istat *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), istat);
    return do_disk_stat(fd, pistat);
}

//
// close file
//
ssize_t sys_user_close(int fd) { return do_close(fd); }

//
// lib call to opendir
//
ssize_t sys_user_opendir(char *pathva)
{
    int hart_id = read_tp();
    char *pathpa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), pathva);
    return do_opendir(pathpa);
}

//
// lib call to readdir
//
ssize_t sys_user_readdir(int fd, struct dir *vdir)
{
    int hart_id = read_tp();
    struct dir *pdir = (struct dir *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), vdir);
    return do_readdir(fd, pdir);
}

//
// lib call to mkdir
//
ssize_t sys_user_mkdir(char *pathva)
{
    int hart_id = read_tp();
    char *pathpa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), pathva);
    return do_mkdir(pathpa);
}

//
// lib call to closedir
//
ssize_t sys_user_closedir(int fd) { return do_closedir(fd); }

//
// lib call to link
//
ssize_t sys_user_link(char *vfn1, char *vfn2)
{
    int hart_id = read_tp();
    char *pfn1 = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)vfn1);
    char *pfn2 = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)vfn2);
    return do_link(pfn1, pfn2);
}

//
// lib call to unlink
//
ssize_t sys_user_unlink(char *vfn)
{
    int hart_id = read_tp();
    char *pfn = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)vfn);
    return do_unlink(pfn);
}

ssize_t sys_user_exec(char *path, char *arg)
{
    int hart_id = read_tp();
    char *pfn = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)path);
    char *parg = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)arg);
    return do_exec(pfn, parg);
}

ssize_t sys_user_wait(int pid) { return do_wait(pid); }

ssize_t sys_user_sem_init(int sem) { return sem_init(sem); }

ssize_t sys_user_sem_acquire(uint64 sem)
{
    __acquire(sem);
    return 0;
}

ssize_t sys_user_sem_release(uint64 sem)
{
    __release(sem);
    return 0;
}

// ADD:lab2_challenge2
uint64 sys_user_malloc(uint64 n) { return (uint64)vm_malloc(n); }

uint64 sys_user_free(uint64 va)
{
    vm_free(va);
    return 0;
}

ssize_t sys_user_backtrace(uint64 depth)
{
    int hart_id = read_tp();
    uint64 fp = (current[hart_id]->trapframe->regs.s0);
    uint64 pa = (uint64)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)(fp - 8));
    fp = *(uint64 *)(pa);
    for (uint64 i = 0; i < depth; i++)
    {
        pa = (uint64)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), (void *)fp - 8);
        const uint64 ra = *(uint64 *)(pa);
        func_name_printer(ra);
        fp = *(uint64 *)(pa - 8);
        if (fp == 0)
        {
            break;
        }
    }
    return 0;
}
long sys_user_ccwd(char *pathva)
{
    int hart_id = read_tp();
    char *pathpa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), pathva);
    return do_change_cwd(pathpa);
}


long sys_user_rcwd(char *pathva)
{
    int hart_id = read_tp();
    char *pathpa = (char *)user_va_to_pa((pagetable_t)(current[hart_id]->pagetable), pathva);
    return do_read_cwd(pathpa);
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7)
{
    switch (a0)
    {
    case SYS_user_print:
        return sys_user_print((const char *)a1, a2);
    case SYS_user_exit:
        return sys_user_exit(a1);
    // added @lab2_2
    case SYS_user_allocate_page:
        return sys_user_allocate_page();
    case SYS_user_free_page:
        return sys_user_free_page(a1);
    case SYS_user_fork:
        return sys_user_fork();
    case SYS_user_yield:
        return sys_user_yield();
    // added @lab4_1
    case SYS_user_open:
        return sys_user_open((char *)a1, a2);
    case SYS_user_read:
        return sys_user_read(a1, (char *)a2, a3);
    case SYS_user_write:
        return sys_user_write(a1, (char *)a2, a3);
    case SYS_user_lseek:
        return sys_user_lseek(a1, a2, a3);
    case SYS_user_stat:
        return sys_user_stat(a1, (struct istat *)a2);
    case SYS_user_disk_stat:
        return sys_user_disk_stat(a1, (struct istat *)a2);
    case SYS_user_close:
        return sys_user_close(a1);
    // added @lab4_2
    case SYS_user_opendir:
        return sys_user_opendir((char *)a1);
    case SYS_user_readdir:
        return sys_user_readdir(a1, (struct dir *)a2);
    case SYS_user_mkdir:
        return sys_user_mkdir((char *)a1);
    case SYS_user_closedir:
        return sys_user_closedir(a1);
    // added @lab4_3
    case SYS_user_link:
        return sys_user_link((char *)a1, (char *)a2);
    case SYS_user_unlink:
        return sys_user_unlink((char *)a1);
    case SYS_user_exec:
        return sys_user_exec((char *)a1, (char *)a2);
    case SYS_user_wait:
        return sys_user_wait(a1);
    case SYS_user_gets:
        return sys_user_gets((char *)a1, a2);
    case SYS_user_sem_init:
        return sys_user_sem_init(a1);
    case SYS_user_sem_P:
        return sys_user_sem_acquire(a1);
    case SYS_user_sem_V:
        return sys_user_sem_release(a1);
    case SYS_user_malloc:
        return sys_user_malloc(a1); // a1为申请的字节数
    case SYS_user_free:
        return sys_user_free(a1); // a1为虚拟地址
    case SYS_user_printpa:
        return sys_user_printpa(a1);
    case SYS_user_backtrace:
        return sys_user_backtrace(a1);
    case SYS_user_rcwd:
        return sys_user_rcwd((char *)a1);
    case SYS_user_ccwd:
        return sys_user_ccwd((char *)a1);
    default:
        panic("Unknown syscall %ld \n", a0);
    }
}
