#ifndef VM_H
#define VM_H

#include "sched.h"

#define offset (0xffffffe000000000 - 0x80000000)

#define P_KERNEL 0x80000000
#define V_KERNEL 0xffffffe000000000
#define MAP_SIZE 0x1000000 //16MB
#define UART_ADDR 0x10000000

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm);
void paging_init();

#endif
