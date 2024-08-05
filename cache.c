#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define B *1
#define KB *1024
#define MB *1024 * 1024
#define GB *1024 * 1024 * 1024
#define CACHE_LINE_SIZE 2
#define SET_SIZE 8

typedef struct {
    uint8_t valid;
    uint32_t tag;
    uint8_t data[16 B];
    uint8_t age;
} cache_line_t;

typedef struct {
    cache_line_t cache_line[CACHE_LINE_SIZE];
} set_t;

typedef struct {
    set_t set[SET_SIZE];
} cache_t;

void cache_init(cache_t *cache) { memset((void *)cache, 0, sizeof(cache_t)); }

void cache_miss(uint32_t p_addr, cache_t *cache) {
    uint32_t tag = p_addr >> 7;
    uint32_t set_index = (p_addr >> 4) & 0x7;
    int min_age_pos = 0;
    uint8_t min_age = cache->set[set_index].cache_line[0].age;
    for (int i = 0; i < CACHE_LINE_SIZE; i++) {
        uint8_t age = cache->set[set_index].cache_line[i].age;
        if (age < min_age) {
            min_age = age;
            min_age_pos = i;
        }
        cache->set[set_index].cache_line[i].age = age >> 1;
    }
    cache->set[set_index].cache_line[min_age_pos].age = (min_age | 0xf0);
    cache->set[set_index].cache_line[min_age_pos].tag = tag;
    cache->set[set_index].cache_line[min_age_pos].valid = 1;
}

void cache_visit(uint32_t p_addr, cache_t *cache) {
    printf("%08x\n", p_addr);
    uint32_t tag = p_addr >> 7;
    uint32_t set_index = (p_addr >> 4) & 0x7;
    uint32_t offset = p_addr & 0xf;

    bool miss = true;
    for (int i = 0; i < CACHE_LINE_SIZE; i++) {
        if (tag == cache->set[set_index].cache_line[i].tag &&
            cache->set[set_index].cache_line[i].valid == 1) {
            miss = false;
            printf("cache hit!\n");
            break;
        }
    }
    if (miss) {
        cache_miss(p_addr, cache);
        printf("cache miss\n");
    }
}

void cache_print(cache_t *cache) {
    for (int i = 0; i < SET_SIZE; i++) {
        printf("set %d:  ", i);
        for (int j = 0; j < CACHE_LINE_SIZE; j++) {
            printf("line %d: tag : %08x vaild: %d    ", j,
                   cache->set[i].cache_line[j].tag,
                   cache->set[i].cache_line[j].valid);
        }
        printf("\n");
    }
}

int main() {
    uint64_t mem =
        (uint64_t)mmap((void *)0x17f500000, 64 MB, PROT_WRITE | PROT_READ,
                       MAP_PRIVATE | MAP_ANON, 0, 0);

    uint32_t cache_size = sizeof(cache_t);
    cache_t *cache = (cache_t *)malloc(sizeof(cache_t));
    cache_init(cache);
    cache_print(cache);
    int32_t *a = (int32_t *)mem;

    for (int i = 0; i < 10; i++) {
        uint64_t addr = (uint64_t)&a[i];
        cache_visit(addr, cache);
    }
    cache_print(cache);
    return 0;
}
