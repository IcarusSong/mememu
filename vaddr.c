#include "mmu.h"
#include <stdint.h>

uint32_t next_free_frame = 0;

int page_fault_count = 0;

void page_table_init(page_table_t *page_table) {
    memset(page_table, 0, sizeof(page_table_t));
}
static inline void read_disk() {}

static inline uint32_t find_age_page(page_table_t *page_table) {
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

static inline void handle_page_fault(uint32_t vaddr, page_table_t *page_table) {
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
    page_table->entries[vpn].age |= 0x80000000;
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
            page_table->entries[vpn].age |= 0x80000000;
        }
    }
    ppn = page_table->entries[vpn].frame;
    paddr = (ppn << 12) | vpo;
    return paddr;
}
void print_page_fault_count() {
    printf("page_fault_count : %d    \n", page_fault_count);
}

uint32_t tlb_trans_addr(uint32_t vaddr, tlb_t *tlb, page_table_t *page_table) {
    uint32_t vpn = vaddr >> 12;
    uint32_t vpo = vaddr & 0xfff;
    uint32_t tlbt = vpn >> TLB_SET_BITS;
    uint32_t tlbi = vpn & (TLB_SET_NUM - 1);
    uint32_t ppn;
    uint32_t paddr;
    bool tlb_hit = false;
    int pos = 0;
    for (int i = 0; i < TLB_LINE_NUM; i++) {
        if (tlb->tlb_set[tlbi].tlb_line[i].tlbt == tlbt &&
            tlb->tlb_set[tlbi].tlb_line[i].valid) {
            ppn = tlb->tlb_set[tlbi].tlb_line[i].frame;
            paddr = (ppn << 12) | vpo;
            tlb_hit = true;
            pos = i;
            break;
        }
    }
    if (tlb_hit) {
        if (!(tlb->tlb_set[tlbi].tlb_line[pos].age >> 7)) {
            for (int i = 0; i < TLB_LINE_NUM; i++) {
                tlb->tlb_set[tlbi].tlb_line[i].age >>= 1;
            }
            tlb->tlb_set[tlbi].tlb_line[pos].age |= 0x80;
        }
    } else {
        paddr = vaddr_trans_paddr(vaddr, page_table);
        int min_age_pos = 0;
        uint32_t min_age = tlb->tlb_set[tlbi].tlb_line[0].age;
        for (int i = 0; i < TLB_LINE_NUM; i++) {
            if (min_age > tlb->tlb_set[tlbi].tlb_line[i].age) {
                min_age = tlb->tlb_set[tlbi].tlb_line[i].age;
                min_age_pos = i;
                tlb->tlb_set[tlbi].tlb_line[i].age >>= 1;
            }
        }
        tlb->tlb_set[tlbi].tlb_line[min_age_pos].frame = (paddr >> 12);
        tlb->tlb_set[tlbi].tlb_line[min_age_pos].valid = 1;
        tlb->tlb_set[tlbi].tlb_line[min_age_pos].age |= 0x80;
    }
    return paddr;
}
