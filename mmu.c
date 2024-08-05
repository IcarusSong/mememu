#include "mmu.h"

int main() {
    uint64_t mem =
        (uint64_t)mmap((void *)0x100000000, PMEM_SIZE, PROT_WRITE | PROT_READ,
                       MAP_PRIVATE | MAP_ANON, -1, 0);

    cache_t *cache = (cache_t *)malloc(sizeof(cache_t));
    cache_init(cache);
    page_table_t *page_table = (page_table_t *)malloc(sizeof(page_table_t));
    uint8_t *_a = (uint8_t *)malloc(sizeof(uint8_t) * 2 GB);
    int32_t *a = (int32_t *)_a;
    for (long long i = 0; i < (1 << 20); i++) {
        uint32_t vaddr = (uint32_t)(uint64_t)(&a[i]);
        vaddr_trans_paddr(vaddr, page_table);
    }
    print_page_fault_count();
    munmap((void *)mem, PMEM_SIZE);
    return 0;
}
