#include "mmu.h"
#include <stdint.h>

int main() {
    uint64_t mem =
        (uint64_t)mmap((void *)0x100000000, PMEM_SIZE, PROT_WRITE | PROT_READ,
                       MAP_PRIVATE | MAP_ANON, -1, 0);

    cache_t *cache = (cache_t *)malloc(sizeof(cache_t));
    cache_init(cache);
    page_table_t *page_table = (page_table_t *)malloc(sizeof(page_table_t));
    tlb_t *tlb = (tlb_t *)malloc(sizeof(tlb_t));

    uint8_t *_a = (uint8_t *)malloc(sizeof(uint8_t) * 2 GB);
    // int32_t *a = (int32_t *)_a;
    int a[256][256];
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            uint32_t vaddr = (uint32_t)(uint64_t)(&a[i]);
            uint32_t paddr = tlb_trans_addr(vaddr, tlb, page_table);
            // printf("vaddr: %08x, paddr: %08x\n", vaddr, paddr);
            cache_visit(paddr, cache);
        }
    }
    // for (long long i = 0; i < (1 << 8); i++) {
    //     uint32_t vaddr = (uint32_t)(uint64_t)(&a[i]);
    //     uint32_t paddr = tlb_trans_addr(vaddr, tlb, page_table);
    //     // printf("vaddr: %08x, paddr: %08x\n", vaddr, paddr);
    //     cache_visit(paddr, cache);
    // }
    print_page_fault_count();
    cache_print(cache);
    print_hit_rate();
    munmap((void *)mem, PMEM_SIZE);
    free(cache);
    free(tlb);
    free(_a);
    free(page_table);
    return 0;
}
