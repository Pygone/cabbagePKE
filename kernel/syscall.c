/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "symbol.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"

#include "spike_interface/spike_utils.h"


//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n)
{
	sprint(buf);
	return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code)
{
	sprint("User exit with code:%d.\n", code);
	// in lab1, PKE considers only one app (one process).
	// therefore, shutdown the system when the app calls exit()
	shutdown(code);
}

void func_name_printer(uint64 ret_addr)
{
	// 初始化左右边界
	int left = 0;
	int right = sym_count - 1;
	// 当左边界小于等于右边界时，继续查找
	while (left <= right) {
		// 计算中间位置
		int mid = left + (right - left) / 2;

		// 获取中间位置符号的起始地址和结束地址
		uint64 begin_addr = symbols[mid].st_value;
		uint64 end_addr = symbols[mid].st_value + symbols[mid].st_size;

		// 如果返回地址在起始地址和结束地址之间，打印符号名并返回
		if (begin_addr <= ret_addr && ret_addr < end_addr) {
//			sprint("0x%lx: begin_addr: 0x%lx, end_addr: 0x%lx, ", ret_addr, begin_addr, end_addr);
			sprint("%s\n", sym_names[mid]);
			return;
		}

		// 如果返回地址小于起始地址，更新右边界
		if (begin_addr > ret_addr) {
			right = mid - 1;
		}
			// 如果返回地址大于结束地址，更新左边界
		else {
			left = mid + 1;
		}
	}
}

ssize_t sys_user_backtrace(uint64 depth)
{
	uint64 fp = (current->trapframe->regs.s0);
	fp = *(uint64*) (fp - 8);
	uint64 ra, prev_fp;
	for (uint64 i = 0; i < depth; i++) {
		ra = *(uint64*) (fp - 8);
		prev_fp = *(uint64*) (fp - 16);
		func_name_printer(ra);
		if (prev_fp == 0) {
			break;
		}
		fp = prev_fp;
	}

	return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7)
{
	switch (a0) {
		case SYS_user_print:
			return sys_user_print((const char*) a1, a2);
		case SYS_user_exit:
			return sys_user_exit(a1);
		case SYS_user_backtrace:
			return sys_user_backtrace(a1);
		default:
			panic("Unknown syscall %ld \n", a0);
	}
}
