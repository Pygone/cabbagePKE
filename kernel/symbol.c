#include "elf.h"
#include "spike_interface/spike_utils.h"
#include "util/string.h"
elf_symbol symbols[1024];
char sym_names[1024][32];
int sym_count = 0;

void quickSort(int l, int r)
{
    if (l >= r) return;
    int i = l, j = r;
    elf_symbol x = symbols[l];
    char temp_name[32];
    strcpy(temp_name, sym_names[l]);
    while (i < j) {
        while (i < j && symbols[j].st_value >= x.st_value) j--;
        if (i < j) {
            symbols[i] = symbols[j];
            strcpy(sym_names[i], sym_names[j]);
        }
        while (i < j && symbols[i].st_value < x.st_value) i++;
        if (i < j) {
            symbols[j] = symbols[i];
            strcpy(sym_names[j], sym_names[i]);
        }
    }
    symbols[i] = x;
    strcpy(sym_names[i], temp_name);
    quickSort(l, i - 1);
    quickSort(i + 1, r);
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