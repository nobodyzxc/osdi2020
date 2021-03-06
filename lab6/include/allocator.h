#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "mm.h"

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
  unsigned long addr;
  unsigned long size;
  unsigned long page_number;
  Page *pages;
} ZoneStr, *Zone;

extern Zone buddy_zone;

Zone page_allocate(Zone *zone, unsigned long size);
void zone_init(Zone);

unsigned long startup_allocate(unsigned long size, unsigned long mask);

unsigned long zone_get_free_pages(Zone zone, int order);
void zone_free_pages(Zone zone, unsigned long addr);

void zone_show(Zone zone, unsigned limit);

#define addr2pgidx(_addr, _zone) ((_addr - (_zone->addr)) / PAGE_SIZE)

#define enable_startup_log 1
#define enable_buddy_log 1
#define enable_fixed_log 1
#define enable_varied_log 1

#if enable_startup_log
#define startup_log(...) printfmt(__VA_ARGS__)
#define startup_log_graph(zone) startup_show(zone, 3)
#else
#define startup_log(...) ;
#define startup_log_graph(zone) ;
#endif

#if enable_buddy_log
#define buddy_log(...) printfmt(__VA_ARGS__)
#define buddy_log_graph(zone) zone_show(zone, 3)
#else
#define buddy_log(...) ;
#define buddy_log_graph(zone) ;
#endif

#if enable_fixed_log
#define fixed_log(...) printfmt(__VA_ARGS__)
#define fixed_log_graph(fixed) fixed_show(fixed, 3)
#else
#define fixed_log(...) ;
#define fixed_log_graph(fixed) ;
#endif

#if enable_varied_log
#define varied_log(...) printfmt(__VA_ARGS__)
#define varied_log_graph(varied) varied_show(varied, 3)
#else
#define varied_log(...) ;
#define varied_log_graph(varied) ;
#endif

/* fixed size allocator below */

#define FixedBookTableSzie 512
typedef struct FixedBook_tag {
  unsigned long page_addr;
  unsigned long free_nr;
  char *table;
  struct FixedBook_tag *next;
} FixedBookStr, *FixedBook;

typedef struct FixedAllocator_tag {
  /* free areas of differents sizes */
  unsigned long size;
  unsigned long rs; /* round up size */
  struct FixedAllocator_tag *next;
  FixedBook book;
} FixedAllocatorStr, *FixedAllocator;

unsigned long fixed_get_token(unsigned long size);
void fixed_free_token(unsigned long token);
unsigned long fixed_alloc(unsigned long token);
void fixed_free(unsigned long token, unsigned long addr);
void fixed_show(FixedAllocator fixed, unsigned limit);

typedef struct VariedAllocator_tag {
  FixedAllocator fixed;
  struct VariedAllocator_tag *next;
} VariedAllocatorStr, *VariedAllocator;

unsigned long varied_get_token();
void varied_free_token(unsigned long token);
unsigned long varied_alloc(unsigned long token, unsigned long size);
void varied_free(unsigned long token, unsigned long addr);
void varied_show(VariedAllocator varied, unsigned limit);

unsigned long kmalloc(unsigned long size);
void kfree(unsigned long addr);
unsigned long startup_max_cont_space_left();

#endif
