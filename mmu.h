
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
#define CO 6
#define CI 3
#define CE 0
#define CACHE_LINE_SIZE (1 << CE)
#define SET_SIZE (1 << CI)
#define ARRAY_ROWS 256
#define ARRAY_COLS 256

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
