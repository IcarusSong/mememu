#include "mmu.h"
#include <stdint.h>

int main() {
    uint64_t mem =
        (uint64_t)mmap((void *)0x100000000, PMEM_SIZE, PROT_WRITE | PROT_READ,
                       MAP_PRIVATE | MAP_ANON, -1, 0);

    cache_t *cache = (cache_t *)malloc(sizeof(cache_t));
    cache_init(cache);

    int(*array)[ARRAY_COLS] = (int(*)[ARRAY_COLS])mem;

    for (int i = 0; i < ARRAY_ROWS; i++) {
        for (int j = 0; j < ARRAY_COLS; j++) {
            cache_visit((uint64_t)&array[i][j], cache);
        }
    }
    printf("row-wise access:\n");
    print_hit_rate();
    cache_print(cache);
    cache_init(cache);
    for (int i = 0; i < ARRAY_ROWS; i++) {
        for (int j = 0; j < ARRAY_COLS; j++) {
            cache_visit((uint64_t)&array[j][i], cache);
        }
    }
    printf("col-wise access:\n");
    print_hit_rate();

    return 0;
}
