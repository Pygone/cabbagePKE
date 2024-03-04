/*
 * routines that scan and load a (host) Executable and Linkable Format (ELF) file
 * into the (emulated) memory.
 */

#include "elf.h"
#include "pmm.h"
#include "riscv.h"
#include "spike_interface/spike_utils.h"
#include "string.h"
#include "vfs.h"
#include "vmm.h"

typedef struct elf_info_t
{
    struct file *f;
    process *p;
} elf_info;

//
// the implementation of allocater. allocates memory space for later segment loading.
// this allocater is heavily modified @lab2_1, where we do NOT work in bare mode.
//
static void *elf_alloc_mb(elf_ctx *ctx, uint64 elf_pa, uint64 elf_va, uint64 size)
{
    elf_info *msg = (elf_info *)ctx->info;
    // we assume that size of proram segment is smaller than a page.
    kassert(size < PGSIZE);
    void *pa = alloc_page();
    if (pa == 0)
        panic("uvmalloc mem alloc falied\n");

    memset((void *)pa, 0, PGSIZE);
    user_vm_map((pagetable_t)msg->p->pagetable, elf_va, PGSIZE, (uint64)pa,
                prot_to_type(PROT_WRITE | PROT_READ | PROT_EXEC, 1));

    return pa;
}

//
// actual file reading, using the vfs file interface.
//
static uint64 elf_fpread(elf_ctx *ctx, void *dest, uint64 nb, uint64 offset)
{
    elf_info *msg = (elf_info *)ctx->info;
    vfs_lseek(msg->f, offset, SEEK_SET);
    return vfs_read(msg->f, dest, nb);
}

//
// init elf_ctx, a data structure that loads the elf.
//
elf_status elf_init(elf_ctx *ctx, void *info)
{
    ctx->info = info;

    // load the elf header
    if (elf_fpread(ctx, &ctx->ehdr, sizeof(ctx->ehdr), 0) != sizeof(ctx->ehdr))
        return EL_EIO;

    // check the signature (magic value) of the elf
    if (ctx->ehdr.magic != ELF_MAGIC)
        return EL_NOTELF;

    return EL_OK;
}

// leb128 (little-endian base 128) is a variable-length
// compression algoritm in DWARF
void read_uleb128(uint64 *out, char **off)
{
    uint64 value = 0;
    int shift = 0;
    uint8 b;
    for (;;)
    {
        b = *(uint8 *)(*off);
        (*off)++;
        value |= ((uint64)b & 0x7F) << shift;
        shift += 7;
        if ((b & 0x80) == 0)
            break;
    }
    if (out)
        *out = value;
}
void read_sleb128(int64 *out, char **off)
{
    int64 value = 0;
    int shift = 0;
    uint8 b;
    for (;;)
    {
        b = *(uint8 *)(*off);
        (*off)++;
        value |= ((uint64_t)b & 0x7F) << shift;
        shift += 7;
        if ((b & 0x80) == 0)
            break;
    }
    if (shift < 64 && (b & 0x40))
        value |= -(1 << shift);
    if (out)
        *out = value;
}
// Since reading below types through pointer cast requires aligned address,
// so we can only read them byte by byte
void read_uint64(uint64 *out, char **off)
{
    *out = 0;
    for (int i = 0; i < 8; i++)
    {
        *out |= (uint64)(**off) << (i << 3);
        (*off)++;
    }
}
void read_uint32(uint32 *out, char **off)
{
    *out = 0;
    for (int i = 0; i < 4; i++)
    {
        *out |= (uint32)(**off) << (i << 3);
        (*off)++;
    }
}
void read_uint16(uint16 *out, char **off)
{
    *out = 0;
    for (int i = 0; i < 2; i++)
    {
        *out |= (uint16)(**off) << (i << 3);
        (*off)++;
    }
}

/*
 * analyzis the data in the debug_line section
 *
 * the function needs 3 parameters: elf context, data in the debug_line section
 * and length of debug_line section
 *
 * make 3 arrays:
 * "process->dir" stores all directory paths of code files
 * "process->file" stores all code file names of code files and their directory path index of array "dir"
 * "process->line" stores all relationships map instruction addresses to code line numbers
 * and their code file name index of array "file"
 */
void make_addr_line(elf_ctx *ctx, char *debug_line, uint64 length)
{
    process *p = ((elf_info *)ctx->info)->p;

    p->debugline = debug_line;
    // directory name char pointer array
    p->dir = (char **)((((uint64)debug_line + length + 7) >> 3) << 3);
    int dir_ind = 0, dir_base;
    // file name char pointer array
    p->file = (code_file *)(p->dir + 64);
    int file_ind = 0, file_base;
    // table array
    p->line = (addr_line *)(p->file + 64);
    p->line_ind = 0;
    char *off = debug_line;

    while (off < debug_line + length)
    { // iterate each compilation unit(CU)
        debug_header *dh = (debug_header *)off;
        off += sizeof(debug_header);
        dir_base = dir_ind;
        file_base = file_ind;
        // get directory name char pointer in this CU
        while (*off != 0)
        {
            p->dir[dir_ind++] = off;
            while (*off != 0)
                off++;
            off++;
        }
        off++;
        // get file name char pointer in this CU
        while (*off != 0)
        {
            p->file[file_ind].file = off;
            while (*off != 0)
                off++;
            off++;
            uint64 dir;
            read_uleb128(&dir, &off);
            p->file[file_ind++].dir = dir - 1 + dir_base;
            read_uleb128(NULL, &off);
            read_uleb128(NULL, &off);
        }
        off++;
        addr_line regs;
        regs.addr = 0;
        regs.file = 1;
        regs.line = 1;
        // simulate the state machine op code
        for (;;)
        {
            uint8 op = *(off++);
            switch (op)
            {
            case 0: // Extended Opcodes
                read_uleb128(NULL, &off);
                op = *(off++);
                switch (op)
                {
                case 1: // DW_LNE_end_sequence
                    if (p->line_ind > 0 && p->line[p->line_ind - 1].addr == regs.addr)
                        p->line_ind--;
                    p->line[p->line_ind] = regs;
                    p->line[p->line_ind].file += file_base - 1;
                    p->line_ind++;
                    goto endop;
                case 2: // DW_LNE_set_address
                    read_uint64(&regs.addr, &off);
                    break;
                // ignore DW_LNE_define_file
                case 4: // DW_LNE_set_discriminator
                    read_uleb128(NULL, &off);
                    break;
                }
                break;
            case 1: // DW_LNS_copy
                if (p->line_ind > 0 && p->line[p->line_ind - 1].addr == regs.addr)
                    p->line_ind--;
                p->line[p->line_ind] = regs;
                p->line[p->line_ind].file += file_base - 1;
                p->line_ind++;
                break;
            case 2:
                { // DW_LNS_advance_pc
                    uint64 delta;
                    read_uleb128(&delta, &off);
                    regs.addr += delta * dh->min_instruction_length;
                    break;
                }
            case 3:
                { // DW_LNS_advance_line
                    int64 delta;
                    read_sleb128(&delta, &off);
                    regs.line += delta;
                    break;
                }
            case 4: // DW_LNS_set_file
                read_uleb128(&regs.file, &off);
                break;
            case 5: // DW_LNS_set_column
                read_uleb128(NULL, &off);
                break;
            case 6: // DW_LNS_negate_stmt
            case 7: // DW_LNS_set_basic_block
                break;
            case 8:
                { // DW_LNS_const_add_pc
                    int adjust = 255 - dh->opcode_base;
                    int delta = (adjust / dh->line_range) * dh->min_instruction_length;
                    regs.addr += delta;
                    break;
                }
            case 9:
                { // DW_LNS_fixed_advanced_pc
                    uint16 delta;
                    read_uint16(&delta, &off);
                    regs.addr += delta;
                    break;
                }
                // ignore 10, 11 and 12
            default:
                { // Special Opcodes
                    int adjust = op - dh->opcode_base;
                    int addr_delta = (adjust / dh->line_range) * dh->min_instruction_length;
                    int line_delta = dh->line_base + (adjust % dh->line_range);
                    regs.addr += addr_delta;
                    regs.line += line_delta;
                    if (p->line_ind > 0 && p->line[p->line_ind - 1].addr == regs.addr)
                        p->line_ind--;
                    p->line[p->line_ind] = regs;
                    p->line[p->line_ind].file += file_base - 1;
                    p->line_ind++;

                    break;
                }
            }
        }

    endop:;
    }
}

//
// load the elf segments to memory regions.
//
elf_status elf_load(elf_ctx *ctx)
{
    // elf_prog_header structure is defined in kernel/elf.h
    elf_prog_header ph_addr;
    int i, off;

    // traverse the elf program segment headers
    for (i = 0, off = ctx->ehdr.phoff; i < ctx->ehdr.phnum; i++, off += sizeof(ph_addr))
    {
        // read segment headers
        if (elf_fpread(ctx, (void *)&ph_addr, sizeof(ph_addr), off) != sizeof(ph_addr))
            return EL_EIO;

        if (ph_addr.type != ELF_PROG_LOAD)
            continue;
        if (ph_addr.memsz < ph_addr.filesz)
            return EL_ERR;
        if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr)
            return EL_ERR;

        // allocate memory block before elf loading
        void *dest = elf_alloc_mb(ctx, ph_addr.vaddr, ph_addr.vaddr, ph_addr.memsz);

        // actual loading
        if (elf_fpread(ctx, dest, ph_addr.memsz, ph_addr.off) != ph_addr.memsz)
            return EL_EIO;

        // record the vm region in proc->mapped_info. added @lab3_1
        int j;
        for (j = 0; j < PGSIZE / sizeof(mapped_region); j++) // seek the last mapped region
            if ((process *)(((elf_info *)(ctx->info))->p)->mapped_info[j].va == 0x0)
                break;

        ((process *)(((elf_info *)(ctx->info))->p))->mapped_info[j].va = ph_addr.vaddr;
        ((process *)(((elf_info *)(ctx->info))->p))->mapped_info[j].npages = 1;

        // SEGMENT_READABLE, SEGMENT_EXECUTABLE, SEGMENT_WRITABLE are defined in kernel/elf.h
        if (ph_addr.flags == (SEGMENT_READABLE | SEGMENT_EXECUTABLE))
        {
            ((process *)(((elf_info *)(ctx->info))->p))->mapped_info[j].seg_type = CODE_SEGMENT;
        }
        else if (ph_addr.flags == (SEGMENT_READABLE | SEGMENT_WRITABLE))
        {
            ((process *)(((elf_info *)(ctx->info))->p))->mapped_info[j].seg_type = DATA_SEGMENT;
        }
        else
            panic("unknown program segment encountered, segment flag:%d.\n", ph_addr.flags);

        ((process *)(((elf_info *)(ctx->info))->p))->total_mapped_region++;
    }

    return EL_OK;
}
static char debug_line[0x4000];

static void elf_load_info(elf_ctx *ctx)
{
    // 定义各种节区头部
    elf_sect_header sym_section_header = {0};
    elf_sect_header str_section_header = {0};
    elf_sect_header shstr_section_header;
    elf_sect_header section_header;

    // 获取节区数量
    const uint16 sect_num = ctx->ehdr.shnum;
    // 计算节区字符串表的偏移量
    const uint64 shstr_offset = ctx->ehdr.shoff + ctx->ehdr.shstrndx * ctx->ehdr.shentsize;
    // 读取节区字符串表头部
    elf_fpread(ctx, (void *)&shstr_section_header, ctx->ehdr.shentsize, shstr_offset);
    // 定义字符串表
    char str_table[shstr_section_header.size];
    // 获取节区字符串表的偏移量
    const uint64 shstr_sect_off = shstr_section_header.offset;
    // 读取节区字符串表
    elf_fpread(ctx, &str_table, shstr_section_header.size, shstr_sect_off);

    // 遍历所有节区
    for (int i = 0; i < sect_num; i++)
    {
        // 读取节区头部
        elf_fpread(ctx, (void *)&section_header, ctx->ehdr.shentsize, ctx->ehdr.shoff + i * ctx->ehdr.shentsize);
        // 获取节区类型
        const uint32 type = section_header.type;
        // 如果是符号表，保存符号表头部
        if (type == SHT_SYMTAB)
        {
            sym_section_header = section_header;
            // 如果是字符串表，并且名字是.strtab，保存字符串表头部
        }
        else if (type == SHT_STRTAB && strcmp(str_table + section_header.name, ".strtab") == 0)
        {
            str_section_header = section_header;
        }
        else if (type == 0x1 && strcmp(str_table + section_header.name, ".debug_line") == 0)
        {
            elf_fpread(ctx, (void *)&debug_line, section_header.size, section_header.offset);
            make_addr_line(ctx, debug_line, section_header.size);
        }
    }

    // 获取字符串表的偏移量
    uint64 str_sect_off = str_section_header.offset;
    // 计算符号数量
    uint64 sym_num = sym_section_header.size / sizeof(elf_symbol);
    int count = 0;
    // 遍历所有符号
    for (int i = 0; i < sym_num; i++)
    {
        // 读取符号
        elf_symbol symbol;
        elf_fpread(ctx, (void *)&symbol, sizeof(symbol), sym_section_header.offset + i * sizeof(elf_symbol));
        // 如果符号名字不为空，并且符号类型是函数
        if (symbol.st_name != 0 && (symbol.st_info & 0x0f) == STT_FUNC)
        {
            // 读取符号名字
            char symname[32];
            elf_fpread(ctx, (void *)&symname, sizeof(symname), str_sect_off + symbol.st_name);
            // 保存符号和符号名字
            symbols[count++] = symbol;
            strcpy(sym_names[count - 1], symname);
        }
    }
    // 保存符号数量
    sym_count = count;
    quickSort(0, sym_count - 1);
}


//
// load the elf of user application, by using the spike file interface.
//
void load_bincode_from_host_elf(process *p, char *filename)
{

    // elf loading. elf_ctx is defined in kernel/elf.h, used to track the loading process.
    elf_ctx elfloader;
    // elf_info is defined above, used to tie the elf file and its corresponding process.
    elf_info info;

    info.f = vfs_open(filename, O_RDONLY, vfs_root_dentry);
    info.p = p;
    // IS_ERR_VALUE is a macro defined in spike_interface/spike_htif.h
    if (IS_ERR_VALUE(info.f))
        panic("Fail on openning the input application program.\n");

    // init elfloader context. elf_init() is defined above.
    if (elf_init(&elfloader, &info) != EL_OK)
        panic("fail to init elfloader.\n");

    // load elf. elf_load() is defined above.
    if (elf_load(&elfloader) != EL_OK)
        panic("Fail on loading elf.\n");
    elf_load_info(&elfloader);
    // entry (virtual, also physical in lab1_x) address
    p->trapframe->epc = elfloader.ehdr.entry;
    p->trapframe->regs.tp = read_tp();
    // close the vfs file
    vfs_close(info.f);
}
