#ifndef __MM_H__
#define __MM_H__

#pragma once

/* mapping level */
/* 0 for 2 lv translation */
/* 1 for 3 lv translation */
/* 2 for 4 lv translation */
#include "mmu.h"

#define MLV 1

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define DEVICE_BASE 0x3F000000
#define LOW_MEMORY    (2 * SECTION_SIZE)
#define HIGH_MEMORY   DEVICE_BASE
#define PAGING_MEMORY (HIGH_MEMORY - LOW_MEMORY)

/* refer */

#define PG_DIR_SIZE			(3 * PAGE_SIZE)

#define PHYS_MEMORY_SIZE 		0x40000000

#define VA_START  0xffff000000000000
#define PAGE_MASK 0xfffffffffffff000
#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)
#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#ifndef __ASSEMBLER__




#include "task.h"

typedef struct page_tag {
  enum {
    empty,
    used,
    reserved
  } status;
  unsigned order;
} Page;

unsigned long get_free_page();
void free_page(unsigned long p);
void free_task_pages(Task *);
int map_page(Task *task, unsigned long va, unsigned long page, unsigned long attr);
void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long src, unsigned long dst, unsigned long n);

int copy_virt_memory(Task *dst);
unsigned long allocate_kernel_page();
unsigned long allocate_user_page_with_attr(
    Task *task, unsigned long va, unsigned long attr);

#define allocate_user_page(task, va) \
  allocate_user_page_with_attr(task, va, MMU_PTE_FLAGS)

void mark_reserved_pages(unsigned long end);

void *mmap(void* addr, unsigned long len,
    int prot, int flags, int file_start, int file_offset);

/* 4 mb limit for user process */
#define USER_MEM_LIMIT (4 << 20)
/* 16 kb limit for user stack  */
#define USER_STK_LIMIT (16 << 10)

/* buddy system */
#define MAX_ORDER 10

typedef struct cdr_tag {
  unsigned long val;
  struct cdr_tag *cdr;
} CdrStr, *Cdr;

struct free_area {
  Cdr                free_list;
  unsigned long      nr_free;
};

typedef struct zone_tag {
  /* free areas of differents sizes */
  struct free_area        free_area[MAX_ORDER];
} ZoneStr, *Zone;

extern Zone const buddy_zone;

void zone_init(Zone);
unsigned long zone_get_free_pages(Zone zone, int order);
void zone_free_pages(Zone zone, unsigned long addr);

void zone_show(Zone zone, unsigned long cnt);

#define addr2pgidx(addr) ((addr - ALOC_BEG) / PAGE_SIZE)

#define enable_buddy_log 1

#if enable_buddy_log
#define buddy_log(...) printfmt(__VA_ARGS__)
#define buddy_log_graph(zone) zone_show(zone, 3)
#else
#define buddy_log(...) ;
#define buddy_log_graph(zone) ;
#endif

#endif /* __ASSEMBLER__ */
#endif
