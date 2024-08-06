#include "mmu.h"

int hit_count, miss_count, visit_count;

void cache_init(cache_t *cache) {
    memset((void *)cache, 0, sizeof(cache_t));
    visit_count = miss_count = hit_count = 0;
}

void cache_miss(uint32_t p_addr, cache_t *cache) {
    uint32_t tag = p_addr >> (CO + CI);
    uint32_t set_index = (p_addr >> CO) & ((1U << CI) - 1);
    int min_age_pos = 0;
    uint8_t min_age = cache->set[set_index].cache_line[0].age;
    for (int i = 0; i < CACHE_LINE_SIZE; i++) {
        uint8_t age = cache->set[set_index].cache_line[i].age;
        if (age < min_age) {
            min_age = age;
            min_age_pos = i;
        }
        cache->set[set_index].cache_line[i].age >>= 1;
    }
    cache->set[set_index].cache_line[min_age_pos].age |= 0xf0;
    cache->set[set_index].cache_line[min_age_pos].tag = tag;
    cache->set[set_index].cache_line[min_age_pos].valid = 1;
}

void cache_visit(uint32_t p_addr, cache_t *cache) {
    uint32_t tag = p_addr >> (CO + CI);
    uint32_t set_index = (p_addr >> CO) & ((1U << CI) - 1);
    // uint32_t offset = p_addr & ((1U << CO) - 1);

    bool miss = true;
    visit_count++;
    for (int i = 0; i < CACHE_LINE_SIZE; i++) {
        if (tag == cache->set[set_index].cache_line[i].tag &&
            cache->set[set_index].cache_line[i].valid == 1) {
            miss = false;
            hit_count++;
            break;
        }
    }
    if (miss) {
        miss_count++;
        cache_miss(p_addr, cache);
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

void print_hit_rate() {
    printf(
        "visit count: %d, hit count: %d, miss count: %d, hit rate: %.4f %%\n",
        visit_count, hit_count, miss_count,
        (double)hit_count / visit_count * 100);
}
