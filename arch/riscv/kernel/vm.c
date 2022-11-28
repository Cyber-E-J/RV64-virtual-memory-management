#include "vm.h"
#include "put.h"

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

#define offset (0xffffffe000000000 - 0x80000000)

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm) {
  /* pgtbl: 根页表基地址
     va: 虚拟地址
     pa: 物理地址
     sz: 映射大小
     perm: 权限
   */

  /* pte : 
   63      54 36        28 27        19 18        10 9   8 7 6 5 4 3 2 1 0
   -----------------------------------------------------------------------
  | Reserved |   PPN[2]   |   PPN[1]   |   PPN[0]   | RSW |D|A|G|U|X|W|R|V| 
   -----------------------------------------------------------------------

    Reserved: 全1
    PPN[2]: 9 bits
    PPN[1]: 9 bits
    PPN[0]: 9 bits
   */
  uint64_t address_max;



}

void paging_init() { /* your code */ }
