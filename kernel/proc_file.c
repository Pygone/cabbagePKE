/*
 * Interface functions between file system and kernel/processes. added @lab4_1
 */

#include "proc_file.h"

#include "elf.h"
#include "hostfs.h"
#include "pmm.h"
#include "process.h"
#include "ramdev.h"
#include "rfs.h"
#include "riscv.h"
#include "sched.h"
#include "spike_interface/spike_file.h"
#include "spike_interface/spike_utils.h"
#include "util/functions.h"
#include "util/string.h"
#include "vmm.h"
//
// initialize file system
//
void fs_init(void)
{
    // initialize the vfs
    vfs_init();

    // register hostfs and mount it as the root
    if (register_hostfs() < 0)
        panic("fs_init: cannot register hostfs.\n");
    struct device *hostdev = init_host_device("HOSTDEV");
    vfs_mount("HOSTDEV", MOUNT_AS_ROOT);

    // register and mount rfs
    if (register_rfs() < 0)
        panic("fs_init: cannot register rfs.\n");
    struct device *ramdisk0 = init_rfs_device("RAMDISK0");
    rfs_format_dev(ramdisk0);
    vfs_mount("RAMDISK0", MOUNT_DEFAULT);
}

//
// initialize a proc_file_management data structure for a process.
// return the pointer to the page containing the data structure.
//
proc_file_management *init_proc_file_management(void)
{
    proc_file_management *pfiles = (proc_file_management *)alloc_page();
    pfiles->cwd = vfs_root_dentry; // by default, cwd is the root
    pfiles->nfiles = 0;

    for (int fd = 0; fd < MAX_FILES; ++fd)
        pfiles->opened_files[fd].status = FD_NONE;

    return pfiles;
}

//
// reclaim the open-file management data structure of a process.
// note: this function is not used as PKE does not actually reclaim a process.
//
void reclaim_proc_file_management(proc_file_management *pfiles)
{
    free_page(pfiles);
    return;
}

//
// get an opened file from proc->opened_file array.
// return: the pointer to the opened file structure.
//
struct file *get_opened_file(int fd)
{
    struct file *pfile = NULL;
    int hart_id = read_tp();
    // browse opened file list to locate the fd
    for (int i = 0; i < MAX_FILES; ++i)
    {
        pfile = &(current[hart_id]->pfiles->opened_files[i]); // file entry
        if (i == fd)
            break;
    }
    if (pfile == NULL)
        panic("do_read: invalid fd!\n");
    return pfile;
}

//
// open a file named as "pathname" with the permission of "flags".
// return: -1 on failure; non-zero file-descriptor on success.
//
int do_open(char *pathname, int flags)
{
    struct file *opened_file = NULL;
    int hart_id = read_tp();
    if ((opened_file = vfs_open(pathname, flags, current[hart_id]->pfiles->cwd)) == NULL)
        return -1;

    int fd = 0;
    if (current[hart_id]->pfiles->nfiles >= MAX_FILES)
    {
        panic("do_open: no file entry for current process!\n");
    }
    struct file *pfile;
    for (fd = 0; fd < MAX_FILES; ++fd)
    {
        pfile = &(current[hart_id]->pfiles->opened_files[fd]);
        if (pfile->status == FD_NONE)
            break;
    }

    // initialize this file structure
    memcpy(pfile, opened_file, sizeof(struct file));

    ++current[hart_id]->pfiles->nfiles;
    return fd;
}

//
// read content of a file ("fd") into "buf" for "count".
// return: actual length of data read from the file.
//
int do_read(int fd, char *buf, uint64 count)
{
    struct file *pfile = get_opened_file(fd);

    if (pfile->readable == 0)
        panic("do_read: no readable file!\n");

    char buffer[count + 1];
    int len = vfs_read(pfile, buffer, count);
    buffer[count] = '\0';
    strcpy(buf, buffer);
    return len;
}

//
// write content ("buf") whose length is "count" to a file "fd".
// return: actual length of data written to the file.
//
int do_write(int fd, char *buf, uint64 count)
{
    struct file *pfile = get_opened_file(fd);

    if (pfile->writable == 0)
        panic("do_write: cannot write file!\n");

    int len = vfs_write(pfile, buf, count);
    return len;
}

//
// reposition the file offset
//
int do_lseek(int fd, int offset, int whence)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_lseek(pfile, offset, whence);
}

//
// read the vinode information
//
int do_stat(int fd, struct istat *istat)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_stat(pfile, istat);
}

//
// read the inode information on the disk
//
int do_disk_stat(int fd, struct istat *istat)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_disk_stat(pfile, istat);
}

//
// close a file
//
int do_close(int fd)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_close(pfile);
}

//
// open a directory
// return: the fd of the directory file
//
int do_opendir(char *pathname)
{
    int hart_id = read_tp();
    struct file *opened_file = NULL;
    if ((opened_file = vfs_opendir(pathname, current[hart_id]->pfiles->cwd)) == NULL)
        return -1;

    int fd = 0;
    struct file *pfile;
    for (fd = 0; fd < MAX_FILES; ++fd)
    {
        pfile = &(current[hart_id]->pfiles->opened_files[fd]);
        if (pfile->status == FD_NONE)
            break;
    }
    if (pfile->status != FD_NONE) // no free entry
        panic("do_opendir: no file entry for current process!\n");

    // initialize this file structure
    memcpy(pfile, opened_file, sizeof(struct file));

    ++current[hart_id]->pfiles->nfiles;
    return fd;
}

//
// read a directory entry
//
int do_readdir(int fd, struct dir *dir)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_readdir(pfile, dir);
}

//
// make a new directory
//
int do_mkdir(char *pathname) { return vfs_mkdir(pathname, current[read_tp()]->pfiles->cwd); }

//
// close a directory
//
int do_closedir(int fd)
{
    struct file *pfile = get_opened_file(fd);
    return vfs_closedir(pfile);
}

//
// create hard link to a file
//
int do_link(char *oldpath, char *newpath) { return vfs_link(oldpath, newpath, current[read_tp()]->pfiles->cwd); }

//
// remove a hard link to a file
//
int do_unlink(char *path) { return vfs_unlink(path, current[read_tp()]->pfiles->cwd); }

int do_exec(char *path, char *arg)
{
    int hart_id = read_tp();
    // check file
    struct file *fp = vfs_open(path, O_RDONLY, current[hart_id]->pfiles->cwd);
    if (fp == NULL)
        return -1;
    else
    {
        vfs_close(fp);
    }
    char pth[128], args[128];
    int args_len = strlen(arg);
    strcpy(pth, path);
    strcpy(args, arg);
    uint64 epc = current[hart_id]->trapframe->epc;
    exec_clean(current[hart_id]);
    uint64 argv_va = current[hart_id]->trapframe->regs.sp - args_len - 1;
    argv_va = argv_va - argv_va % 8; // 按8字节对齐(方便指针指向该位置)
    uint64 argv_pa = (uint64)user_va_to_pa(current[hart_id]->pagetable, (void *)argv_va);
    strcpy((char *)argv_pa, args);

    // 二级指针
    uint64 argvs_va = argv_va -
        8; // 因为目前只考虑一个参数，故而一级指针只构建一个，二级指针的位置目前就设定在一级指针后面，并且这一区域的大小刚好只是一个指针的大小
    uint64 argvs_pa = (uint64)user_va_to_pa(current[hart_id]->pagetable, (void *)argvs_va);
    *(uint64 *)argvs_pa = argv_va; // 存储一级指针的虚地址

    current[hart_id]->trapframe->regs.a0 = 1; // 设置argc的值(此处为1)
    current[hart_id]->trapframe->regs.a1 = argvs_va; // 设置argv的值
    current[hart_id]->trapframe->regs.sp = argvs_va - argvs_va % 16; // 按照16对齐
    load_bincode_from_host_elf(current[hart_id], pth);
    if (current[hart_id]->trapframe->regs.a1 == -1)
        current[hart_id]->trapframe->epc = epc;
    return -1;
}
int do_read_cwd(char *path)
{
    int hart_id = read_tp();
    char path_copy[MAX_PATH_LEN];
    struct dentry *cwd = current[hart_id]->pfiles->cwd;
    struct dentry *parent = cwd->parent;
    strcpy(path, cwd->name);
    if (parent == NULL)
    {
        return 0;
    }
    else
    {
        while (parent != NULL)
        {
            strcpy(path_copy, parent->name);
            strcat(path_copy, path);
            parent = parent->parent;
        }
        strcpy(path, path_copy);
    }
    return 0;
}

int do_change_cwd(char *path)
{
    int hart_id = read_tp();
    struct dentry *parent = current[hart_id]->pfiles->cwd;
    char miss_name[MAX_PATH_LEN];

    // lookup the dir, find its parent direntry
    struct dentry *file_dentry = lookup_final_dentry(path, &parent, miss_name);
    if (!file_dentry)
    {
        return -1;
    }

    if (file_dentry->dentry_inode->type != DIR_I)
    {
        sprint("vfs_chdir: cannot change to a file!\n");
        return -1;
    }

    current[hart_id]->pfiles->cwd = file_dentry;
    return 0;
}
int child_running = 0;
int do_wait_call()
{
    while (!child_running)
    {
        ;
    }
    schedule();
    return 0;
}

int do_test(char *path)
{
    int hart_id = read_tp();
    process *p = alloc_process();
    load_bincode_from_host_elf(p, path);
    p->trapframe->regs.tp = 1;
    p->parent = current[hart_id];
    p->is_child = 1;
    insert_to_ready_queue(p);
    child_running = 1;
    current[hart_id]->status = READY;
    insert_to_ready_queue(current[hart_id]);
    schedule();
    return 0;
}