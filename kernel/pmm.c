#include "pmm.h"
#include "config.h"
#include "memlayout.h"
#include "riscv.h"
#include "spike_interface/atomic.h"
#include "spike_interface/spike_utils.h"
#include "util/functions.h"
#include "util/string.h"
// _end is defined in kernel/kernel.lds, it marks the ending (virtual) address of PKE kernel
extern char _end[];
// g_mem_size is defined in spike_interface/spike_memory.c, it indicates the size of our
// (emulated) spike machine. g_mem_size's value is obtained when initializing HTIF.
extern uint64 g_mem_size;

static uint64 free_mem_start_addr; // beginning address of free memory
static uint64 free_mem_end_addr; // end address of free memory (not included)
int vm_alloc_stage[NCPU] = {0};
typedef struct ref_unit
{
    uint64 pa;
    uint64 ref;
} ref_unit;
static ref_unit ref_list[0x4000];
static int64 ref_total = 0;

int ref_find(uint64 pa)
{
    // 二分搜索
    int left = 0, right = ref_total - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        if (ref_list[mid].pa == pa)
        {
            ref_list[mid].ref++;
            return 1;
        }
        else if (ref_list[mid].pa < pa)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return 0;
}

void ref_insert(uint64 pa)
{
    if (ref_find(pa) == 1)
        return;
    // 二分搜索第一个大于等于pa的位置

    int left = 0, right = ref_total - 1, idx = 0;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        if (ref_list[mid].pa < pa)
        {
            left = mid + 1;
        }
        else
        {
            idx = mid;
            right = mid - 1;
        }
    }
    memmove(&ref_list[idx + 1], &ref_list[idx], (ref_total - idx) * sizeof(ref_unit));
    ref_total++;
    ref_list[idx].pa = pa;
    ref_list[idx].ref = 1;
}

int ref_erase(uint64 pa)
{
    // 二分搜索
    int left = 0, right = ref_total - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        if (ref_list[mid].pa == pa)
        {

            ref_list[mid].ref--;
            if (ref_list[mid].ref == 0)
            {
                memmove(&ref_list[mid], &ref_list[mid + 1], (ref_total - mid - 1) * sizeof(ref_unit));
                ref_total--;
                return 1;
            }
            return 0;
        }
        if (ref_list[mid].pa < pa)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }
    return 1;
}

typedef struct node
{
    struct node *next;
} list_node;

// g_free_mem_list is the head of the list of free physical memory pages
static list_node g_free_mem_list;

//
// actually creates the freepage list. each page occupies 4KB (PGSIZE), i.e., small page.
// PGSIZE is defined in kernel/riscv.h, ROUNDUP is defined in util/functions.h.
//
static void create_freepage_list(uint64 start, uint64 end)
{
    g_free_mem_list.next = 0;
    for (uint64 p = ROUNDUP(start, PGSIZE); p + PGSIZE < end; p += PGSIZE)
        free_page((void *)p);
}

static spinlock_t page_latch_;

//
// place a physical page at *pa to the free list of g_free_mem_list (to reclaim the page)
//
void free_page(void *pa)
{
    spinlock_lock(&page_latch_);

    if (((uint64)pa % PGSIZE) != 0 || (uint64)pa < free_mem_start_addr || (uint64)pa >= free_mem_end_addr)
        panic("free_page 0x%lx \n", pa);

    if (ref_erase((uint64)pa) == 0)
    {
        spinlock_unlock(&page_latch_);
        return;
    }

    // insert a physical page to g_free_mem_list
    list_node *n = (list_node *)pa;
    n->next = g_free_mem_list.next;
    g_free_mem_list.next = n;
    spinlock_unlock(&page_latch_);
}

//
// takes the first free page from g_free_mem_list, and returns (allocates) it.
// Allocates only ONE page!
//
void *alloc_page(void)
{
    spinlock_lock(&page_latch_);
    list_node *n = g_free_mem_list.next;
    uint64 hartid = read_tp();
    if (vm_alloc_stage[hartid])
    {
        // sprint("hartid = %ld: alloc page 0x%x\n", hartid, n);
        ;
    }
    if (n)
        g_free_mem_list.next = n->next;
    ref_insert((uint64)n);
    spinlock_unlock(&page_latch_);
    return (void *)n;
}

//
// pmm_init() establishes the list of free physical pages according to available
// physical memory space.
//
void pmm_init()
{
    // start of kernel program segment
    uint64 g_kernel_start = KERN_BASE;
    uint64 g_kernel_end = (uint64)&_end;

    uint64 pke_kernel_size = g_kernel_end - g_kernel_start;
    sprint("PKE kernel start 0x%lx, PKE kernel end: 0x%lx, PKE kernel size: 0x%lx .\n", g_kernel_start, g_kernel_end,
           pke_kernel_size);

    // free memory starts from the end of PKE kernel and must be page-aligined
    free_mem_start_addr = ROUNDUP(g_kernel_end, PGSIZE);

    // recompute g_mem_size to limit the physical memory space that our riscv-pke kernel
    // needs to manage
    g_mem_size = MIN(PKE_MAX_ALLOWABLE_RAM, g_mem_size);
    if (g_mem_size < pke_kernel_size)
        panic("Error when recomputing physical memory size (g_mem_size).\n");

    free_mem_end_addr = g_mem_size + DRAM_BASE;
    sprint("free physical memory address: [0x%lx, 0x%lx] \n", free_mem_start_addr, free_mem_end_addr - 1);

    sprint("kernel memory manager is initializing ...\n");
    // create the list of free pages
    create_freepage_list(free_mem_start_addr, free_mem_end_addr);
}
