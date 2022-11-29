#include "vm.h"
#include "put.h"

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

#define offset (0xffffffe000000000 - 0x80000000)

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm) {
  /* your code */
  
}

void paging_init() { 
  /* your code */
  //设置映射
  uint64_t *pgtbl = &_end;
  uint64_t pa_start = 0x80000000;
  uint64_t va_start = 0xffffffe000000000;
  uint64_t UART = 0x10000000;
  uint64_t size = 0x1000000;
  uint64_t text_size = 0x2000;
  uint64_t rodata_size = 0x1000;
  uint64_t data_size = size - text_size - rodata_size;

  int perm = 7;
  create_mapping(pgtbl, va_start, pa_start, size, perm);  //高地址映射
  create_mapping(pgtbl, pa_start, pa_start, size, perm);  //kernel等值映射
  create_mapping(pgtbl, UART, UART, size, perm);          //UART等值映射

  //权限保护
  //高地址映射
  create_mapping(pgtbl, text_start+offset, text_start, text_size, 5);  //text XWR = 101
  create_mapping(pgtbl, rodata_start+offset, rodata_start, rodata_size, 1);  //rodata XWR = 001
  create_mapping(pgtbl, data_start+offset, data_start, data_size, 3);  //data XWR = 011
  //kernel等值映射
  create_mapping(pgtbl, text_start, text_start, text_size, 5);  //text XWR = 101
  create_mapping(pgtbl, rodata_start, rodata_start, rodata_size, 1);  //rodata XWR = 001
  create_mapping(pgtbl, data_start, data_start, data_size, 3);  //data XWR = 011
  //UART等值映射
  create_mapping(pgtbl, UART, UART, size, 3);  //data XWR = 011        
}
