#include "mmu.h"
#include <stdint.h>

uint32_t next_free_frame = 0;

int page_fault_count = 0;

void page_table_init(page_table_t *page_table) {
    memset(page_table, 0, sizeof(page_table_t));
}
static void read_disk() {}

uint32_t find_age_page(page_table_t *page_table) {
    uint32_t min_age_pos = 0;
    uint32_t min_age = page_table->entries[0].age;
    for (int i = 0; i < PAGE_NUM; i++) {
        if (min_age > page_table->entries[i].age) {
            min_age = page_table->entries[i].age;
            min_age_pos = i;
        }
        page_table->entries[i].age >>= 1;
    }
    return min_age_pos;
}

void handle_page_fault(uint32_t vaddr, page_table_t *page_table) {
    page_fault_count++;
    uint32_t vpn = vaddr >> 12;
    if (next_free_frame < FRAME_NUM) {
        page_table->entries[vpn].frame = next_free_frame;
        next_free_frame++;
    } else {
        uint32_t age_page_pos = find_age_page(page_table);
        page_table->entries[vpn].frame =
            page_table->entries[age_page_pos].frame;
        page_table->entries[age_page_pos].valid = 0;
    }
    read_disk();
    page_table->entries[vpn].valid = 1;
}

uint32_t vaddr_trans_paddr(uint32_t vaddr, page_table_t *page_table) {
    uint32_t vpn = vaddr >> 12;
    uint32_t vpo = vaddr & 0xfff;
    uint32_t ppn, paddr;

    if (!page_table->entries[vpn].valid) {
        handle_page_fault(vaddr, page_table);
    } else {
        if (!(page_table->entries[vpn].age >> 31)) {
            for (int i = 0; i < PAGE_NUM; i++) {
                page_table->entries[i].age >>= 1;
            }
            page_table->entries[vpn].age |= 0xf0000000;
        }
    }
    ppn = page_table->entries[vpn].frame;
    paddr = (ppn << 12) | vpo;
    return paddr;
}
void print_page_fault_count() {
    printf("page_fault_count : %d    ", page_fault_count);
}
