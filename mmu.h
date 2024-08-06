
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define B *1U
#define KB *1024 * 1U
#define MB *1024 * 1024 * 1U
#define GB *1024 * 1024 * 1024 * 1U
#define CO 6 /* cache offset bits */
#define CI 3 /* cache set index bits */
#define CACHE_LINE_SIZE 2
#define SET_SIZE (1 << CI)

#define PMEM_SIZE 1 GB
#define VMEM_SIZE 4 GB
#define PAEG_SIZE 4 KB
#define PAGE_NUM (1 << 20)
#define FRAME_NUM ((PMEM_SIZE) / (PAEG_SIZE))

#define TLB_LINE_NUM 2
#define TLB_SET_BITS 4
#define TLB_SET_NUM (1U << TLB_SET_BITS)
/**
 * cache.c
 */
typedef struct {
    uint8_t valid;
    uint32_t tag;
    uint8_t data[1 << CO B];
    uint8_t age;
} cache_line_t;

typedef struct {
    cache_line_t cache_line[CACHE_LINE_SIZE];
} set_t;

typedef struct {
    set_t set[SET_SIZE];
} cache_t;

void cache_init(cache_t *);
void cache_miss(uint32_t, cache_t *);
void cache_visit(uint32_t, cache_t *);
void cache_print(cache_t *);
void print_hit_rate();

/**
 * vaddr.c
 */
typedef struct {
    uint8_t page[4 KB];
} frame_t;

typedef struct {
    uint32_t frame;
    uint8_t valid;
    uint32_t age;
} page_table_entry_t;

typedef struct {
    page_table_entry_t entries[PAGE_NUM];
} page_table_t;

typedef struct {
    uint32_t tlbt;
    uint32_t age;
    uint32_t frame;
    uint8_t valid;

} tlb_line_t;

typedef struct {
    tlb_line_t tlb_line[TLB_LINE_NUM];
} tlb_set_t;

typedef struct {
    tlb_set_t tlb_set[TLB_SET_NUM];
} tlb_t;
uint32_t vaddr_trans_paddr(uint32_t, page_table_t *);
void print_page_fault_count();
uint32_t tlb_trans_addr(uint32_t, tlb_t *, page_table_t *);
