#include "vm.h"
#include "put.h"

#define PAGE_SIZE 0x1000 //4kb
#define PAGE_FAULT -1
#define RWX 7

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

#define offset (0xffffffe000000000 - 0x80000000)

int page_count = 0;

uint64_t* trace_pte(uint64_t* pgtbl,uint64_t va);




/* void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz, int perm) */

  /* pgtbl: 根页表基地址
     va: 虚拟地址
     pa: 物理地址
     sz: 映射大小
     perm: 权限 */
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

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm) {

  /* your code */

  uint64_t va_end;
  va_end = va + sz -1;

  for (1; va<= va_end ; va += PAGE_SIZE, pa+=PAGE_SIZE){
    uint64_t * pte_address;

    pte_address = trace_pte(pgtbl, va);
    if(pte_address != PAGE_FAULT){

      /* get ppn by eliminating offset */
      uint64_t ppn = pa >> 12;

      /* set pte : reserved  + ppn + perm */
      *pte_address = 0xffc0000000000000 | (uint64_t)(ppn << 10) | (uint64_t)perm;
    }

  }

  
}



/*   
        RISC-V Sv39 Virtual Address and Physical Address Translation
     38        30 29        21 20        12 11                           0
     ---------------------------------------------------------------------
    |   VPN[2]   |   VPN[1]   |   VPN[0]   |          page offset         |
     ---------------------------------------------------------------------
                            Sv39 virtual address */
uint64_t* trace_pte(uint64_t* pgtbl,uint64_t va){
  /* pgtbl: 根页表基地址
     va: 虚拟地址
   */
  uint64_t* pte_address;

  for(int i = 2; i>=0 ;i--){
    uint64_t pte;
    if(i==2){
      uint64_t vpn_2 = (va >> 30) & 0x1ff;
      pte_address = (uint64_t)(pgtbl + vpn_2);
    }
    if(i==1){
      uint64_t vpn_1 = (va >> 21) & 0x1ff;
      pte_address = (uint64_t)(pgtbl + vpn_1);
    }
    if(i==0){
      uint64_t vpn_0 = (va >> 12) & 0x1ff;
      pte_address = (uint64_t)(pgtbl + vpn_0);
    }
    
    /* if pte is valid */
    if( (*pte_address & 0x1) ){
      /* get ppn by eliminating offset */
      uint64_t ppn = (*pte_address) >> 10;

      /* get next level pgtbl */
      pgtbl = (uint64_t*)(ppn << 12);
    }

    else{
      page_count ++; 
      pgtbl = (uint64_t)(&_end + PAGE_SIZE * page_count);
      uint64_t ppn = ((uint64_t)pgtbl >> 12)&(0xfffffffffff);
      *pte_address = ((uint64_t)(*pte_address) & 0xffc0000000000000) | (ppn << 10) | 0x1 ;
    }
  }

  return pte_address;



}



/* ej implemented */
void paging_init() { 
  uint64_t *pgtbl = &_end;

  /* 映射内核起始的16MB空间到高地址 */

  /*kernel起始的16mb映射到高地址*/
  create_mapping(pgtbl, (uint64_t)V_KERNEL, (uint64_t)P_KERNEL, (uint64_t)MAP_SIZE, RWX);
  /*kernel起始地址的16mb做等值映射*/
  create_mapping(pgtbl,(uint64_t)P_KERNEL, (uint64_t)P_KERNEL, (uint64_t)MAP_SIZE, RWX);
  /*UART等值映射*/
  create_mapping(pgtbl, (uint64_t)UART_ADDR, (uint64_t)UART_ADDR, (uint64_t)MAP_SIZE, RWX);





  
  // create_mapping(pgtbl, (uint64_t)(&text_start + offset), (uint64_t)(&text_start), 0x2000, RWX);

  // create_mapping(pgtbl, (uint64_t)(&rodata_start + offset), (uint64_t)(&rodata_start), 0x2000, RWX);

  // create_mapping(pgtbl, (uint64_t)(&text_start + offset), (uint64_t)(&text_start), 0x2000, RWX);

  // create_mapping(pgtbl, (uint64_t)(&text_start + offset), (uint64_t)(&text_start), 0x2000, RWX);

}


/* js implemented */
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