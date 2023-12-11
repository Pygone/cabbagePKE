/*
 * routines that scan and load a (host) Executable and Linkable Format (ELF) file
 * into the (emulated) memory.
 */

#include "elf.h"
#include "string.h"
#include "riscv.h"
#include "spike_interface/spike_utils.h"

typedef struct elf_info_t {
	spike_file_t* f;
	process* p;
} elf_info;

//
// the implementation of allocater. allocates memory space for later segment loading
//
static void* elf_alloc_mb(elf_ctx* ctx, uint64 elf_pa, uint64 elf_va, uint64 size)
{
	// directly returns the virtual address as we are in the Bare mode in lab1_x
	return (void*) elf_va;
}

//
// actual file reading, using the spike file interface.
//
static uint64 elf_fpread(elf_ctx* ctx, void* dest, uint64 nb, uint64 offset)
{
	elf_info* msg = (elf_info*) ctx->info;
	// call spike file utility to load the content of elf file into memory.
	// spike_file_pread will read the elf file (msg->f) from offset to memory (indicated by
	// *dest) for nb bytes.
	return spike_file_pread(msg->f, dest, nb, offset);
}

//
// init elf_ctx, a data structure that loads the elf.
//
elf_status elf_init(elf_ctx* ctx, void* info)
{
	ctx->info = info;

	// load the elf header
	if (elf_fpread(ctx, &ctx->ehdr, sizeof(ctx->ehdr), 0) != sizeof(ctx->ehdr)) return EL_EIO;

	// check the signature (magic value) of the elf
	if (ctx->ehdr.magic != ELF_MAGIC) return EL_NOTELF;

	return EL_OK;
}

//
// load the elf segments to memory regions as we are in Bare mode in lab1
//
elf_status elf_load(elf_ctx* ctx)
{
	// elf_prog_header structure is defined in kernel/elf.h
	elf_prog_header ph_addr;
	int i, off;

	// traverse the elf program segment headers
	for (i = 0, off = ctx->ehdr.phoff; i < ctx->ehdr.phnum; i++, off += sizeof(ph_addr)) {
		// read segment headers
		if (elf_fpread(ctx, (void*) &ph_addr, sizeof(ph_addr), off) != sizeof(ph_addr)) return EL_EIO;

		if (ph_addr.type != ELF_PROG_LOAD) continue;
		if (ph_addr.memsz < ph_addr.filesz) return EL_ERR;
		if (ph_addr.vaddr + ph_addr.memsz < ph_addr.vaddr) return EL_ERR;

		// allocate memory block before elf loading
		void* dest = elf_alloc_mb(ctx, ph_addr.vaddr, ph_addr.vaddr, ph_addr.memsz);

		// actual loading
		if (elf_fpread(ctx, dest, ph_addr.memsz, ph_addr.off) != ph_addr.memsz)
			return EL_EIO;
	}

	return EL_OK;
}

typedef union {
	uint64 buf[MAX_CMDLINE_ARGS];
	char* argv[MAX_CMDLINE_ARGS];
} arg_buf;

//
// returns the number (should be 1) of string(s) after PKE kernel in command line.
// and store the string(s) in arg_bug_msg.
//
static size_t parse_args(arg_buf* arg_bug_msg)
{
	// HTIFSYS_getmainvars frontend call reads command arguments to (input) *arg_bug_msg
	long r = frontend_syscall(HTIFSYS_getmainvars, (uint64) arg_bug_msg,
		sizeof(*arg_bug_msg), 0, 0, 0, 0, 0);
	kassert(r == 0);

	size_t pk_argc = arg_bug_msg->buf[0];
	uint64* pk_argv = &arg_bug_msg->buf[1];

	int arg = 1;  // skip the PKE OS kernel string, leave behind only the application name
	for (size_t i = 0; arg + i < pk_argc; i++)
		arg_bug_msg->argv[i] = (char*) (uintptr_t) pk_argv[arg + i];

	//returns the number of strings after PKE kernel in command line
	return pk_argc - arg;
}

void load_func_from_host_elf(elf_ctx* ctx)
{
	// 定义各种节区头部
	elf_sect_header sym_section_header;
	elf_sect_header str_section_header;
	elf_sect_header shstr_section_header;
	elf_sect_header section_header;

// 获取节区数量
	uint16 sect_num = ctx->ehdr.shnum;
// 计算节区字符串表的偏移量
	uint64 shstr_offset = ctx->ehdr.shoff + ctx->ehdr.shstrndx * ctx->ehdr.shentsize;
// 读取节区字符串表头部
	elf_fpread(ctx, (void*) &shstr_section_header, ctx->ehdr.shentsize, shstr_offset);
// 定义字符串表
	char str_table[shstr_section_header.sh_size];
// 获取节区字符串表的偏移量
	uint64 shstr_sect_off = shstr_section_header.sh_offset;
// 读取节区字符串表
	elf_fpread(ctx, &str_table, shstr_section_header.sh_size, shstr_sect_off);

// 遍历所有节区
	for (int i = 0; i < sect_num; i++) {
		// 读取节区头部
		elf_fpread(ctx, (void*) &section_header, ctx->ehdr.shentsize, ctx->ehdr.shoff + i * ctx->ehdr.shentsize);
		// 获取节区类型
		uint32 type = section_header.sh_type;
		// 如果是符号表，保存符号表头部
		if (type == SHT_SYMTAB) {
			sym_section_header = section_header;
			// 如果是字符串表，并且名字是.strtab，保存字符串表头部
		} else if (type == SHT_STRTAB && strcmp(str_table + section_header.sh_name, ".strtab") == 0) {
			str_section_header = section_header;
		}
	}

// 获取字符串表的偏移量
	uint64 str_sect_off = str_section_header.sh_offset;
// 计算符号数量
	uint64 sym_num = sym_section_header.sh_size / sizeof(elf_symbol);
	int count = 0;
// 遍历所有符号
	for (int i = 0; i < sym_num; i++) {
		// 读取符号
		elf_symbol symbol;
		elf_fpread(ctx, (void*) &symbol, sizeof(symbol), sym_section_header.sh_offset + i * sizeof(elf_symbol));
		// 如果符号名字不为空，并且符号类型是函数
		if (symbol.st_name != 0 && (symbol.st_info&0x0f) == STT_FUNC) {
			// 读取符号名字
			char symname[32];
			elf_fpread(ctx, (void*) &symname, sizeof(symname), str_sect_off + symbol.st_name);
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
void load_bincode_from_host_elf(process* p)
{
	arg_buf arg_bug_msg;

	// retrieve command line arguements
	size_t argc = parse_args(&arg_bug_msg);
	if (!argc) panic("You need to specify the application program!\n");

	sprint("Application: %s\n", arg_bug_msg.argv[0]);

	//elf loading. elf_ctx is defined in kernel/elf.h, used to track the loading process.
	elf_ctx elfloader;
	// elf_info is defined above, used to tie the elf file and its corresponding process.
	elf_info info;

	info.f = spike_file_open(arg_bug_msg.argv[0], O_RDONLY, 0);
	info.p = p;
	// IS_ERR_VALUE is a macro defined in spike_interface/spike_htif.h
	if (IS_ERR_VALUE(info.f)) panic("Fail on openning the input application program.\n");

	// init elfloader context. elf_init() is defined above.
	if (elf_init(&elfloader, &info) != EL_OK)
		panic("fail to init elfloader.\n");

	// load elf. elf_load() is defined above.
	if (elf_load(&elfloader) != EL_OK) panic("Fail on loading elf.\n");

	// entry (virtual, also physical in lab1_x) address
	p->trapframe->epc = elfloader.ehdr.entry;
	load_func_from_host_elf(&elfloader);
//	sprint("epc:%x\n",elfloader.ehdr.shoff);
	// close the host spike file
	spike_file_close(info.f);

	sprint("Application program entry point (virtual address): 0x%lx\n", p->trapframe->epc);
}
