#include "vm.h"
#include "put.h"

#define PAGE_SIZE 4096L //4kb
#define PAGE_FAULT -1
#define RWX 7

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

#define offset (0xffffffe000000000 - 0x80000000)

int page_count = 1;

uint64_t* trace_pte(uint64_t* pgtbl,uint64_t va);



// /* void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz, int perm) */

//   /* pgtbl: 根页表基地址
//      va: 虚拟地址
//      pa: 物理地址
//      sz: 映射大小
//      perm: 权限 */
//   /* pte : 
//    63      54 36        28 27        19 18        10 9   8 7 6 5 4 3 2 1 0
//    -----------------------------------------------------------------------
//   | Reserved |   PPN[2]   |   PPN[1]   |   PPN[0]   | RSW |D|A|G|U|X|W|R|V| 
//    -----------------------------------------------------------------------

//     Reserved: 全1
//     PPN[2]: 9 bits
//     PPN[1]: 9 bits
//     PPN[0]: 9 bits
//    */

// void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
//                     int perm) {

//   // pgtbl: first address in the third-level page table (uint64_t *) 0x80007000 to 0x80008000

//   uint64_t va_end;
//   va_end = va + sz -1;

//   //go through every page in the 16MB space in the virtual address area. In total 16MB/4KB = 4K pages
//   for (1; va<= va_end ; va += PAGE_SIZE, pa+=PAGE_SIZE){
//     uint64_t * pte_address;

//     // get the address of the Page-Table-Entry
//     // everytime goes up by 8(B) in the base pgtbl
//     pte_address = trace_pte(pgtbl, va);
//     if(pte_address != PAGE_FAULT){

//       /* get ppn by eliminating offset */
//       uint64_t ppn = pa >> 12;

//       /* set pte : reserved(10)  + ppn (9c^9^9) + perm (10)*/
//       /* pte size:  64bit , 8B  */
//       *pte_address = 0xffc0000000000000 | (uint64_t)(ppn << 10) | (uint64_t)perm;
//     }

//   }

  
// }



// /*   
//         RISC-V Sv39 Virtual Address and Physical Address Translation
//      38        30 29        21 20        12 11                           0
//      ---------------------------------------------------------------------
//     |   VPN[2]   |   VPN[1]   |   VPN[0]   |          page offset         |
//      ---------------------------------------------------------------------
//                             Sv39 virtual address */
// uint64_t* trace_pte(uint64_t* pgtbl,uint64_t va){
//   /* pgtbl: 根页表基地址
//      va: 虚拟地址
//    */
//   uint64_t* pte_address;

//   for(int i = 2; i>0 ;i--){
//     uint64_t pte;
//     if(i==2){
//       uint64_t vpn_2 = (va >> 30) & 0x1ff;
//       pte_address = (uint64_t)(pgtbl + vpn_2);
//     }
//     if(i==1){
//       uint64_t vpn_1 = (va >> 21) & 0x1ff;
//       pte_address = (uint64_t)(pgtbl + vpn_1);
//     }

//     /* 当i为0时，应该回到原本的函数处理 */
//     // if(i==0){
//     //   uint64_t vpn_0 = (va >> 12) & 0x1ff;
//     //   pte_address = (uint64_t)(pgtbl + vpn_0);
//     // }
    
//     /* if pte is valid */
//     if( (*pte_address & 0x1) ){
//       /* get ppn by eliminating offset */
//       uint64_t ppn = (*pte_address) >> 10;

//       /* get next level pgtbl */
//       pgtbl = (uint64_t*)(ppn << 12);
//     }

//     else{
//       page_count ++; 

//       if(PAGE_SIZE * page_count <= MAP_SIZE){
//         pgtbl = (uint64_t)(&_end + PAGE_SIZE * page_count);
//         uint64_t ppn = ((uint64_t)pgtbl >> 12)&(0xfffffffffff);
//         *pte_address = ((uint64_t)(*pte_address) & 0xffc0000000000000) | (ppn << 10) | 0x1 ;
//       }
      
//       else{
//         puts("Out of Storage.\n");
//         return PAGE_FAULT;
//       }
//     }

//   }

//   uint64_t vpn0 = ((uint64_t)va >> 12)&(0x1ff);
//   return &pgtbl[vpn0];

// }



/* ej implemented */
void paging_init() { 
  uint64_t *pgtbl = &_end;
  // pgtbl = (uint64_t *) 0x80007000

  /* initialize all pagetables */

  for (int i = 0; i < 512; i++)
      pgtbl[i] = 0;

  /* 映射内核起始的16MB空间到高地址 */
  /* kernel起始的16mb映射到高地址*/

  // P --> V
  // create_mapping(pgtbl, (uint64_t)V_KERNEL, (uint64_t)P_KERNEL, (uint64_t)MAP_SIZE, RWX);

  for (uint64_t va = V_KERNEL; va < V_KERNEL + MAP_SIZE; va = va + PAGE_SIZE)
  {
        create_mapping(pgtbl, va, va - offset, PAGE_SIZE, RWX);
        create_mapping(pgtbl, va - offset, va - offset, PAGE_SIZE, RWX);
  }

  // P --> P
  /*kernel起始地址的16mb做等值映射*/
  // create_mapping(pgtbl,(uint64_t)P_KERNEL, (uint64_t)P_KERNEL, (uint64_t)MAP_SIZE, RWX);

  for(uint64_t pa = P_KERNEL; pa < P_KERNEL + MAP_SIZE; pa = pa + PAGE_SIZE)
    create_mapping(pgtbl, pa, pa, PAGE_SIZE, RWX);
  

  // UART --> UART
  /*UART等值映射*/
  create_mapping(pgtbl, (uint64_t)UART_ADDR, (uint64_t)UART_ADDR, (uint64_t)MAP_SIZE, RWX);

}


/* js implemented */
// void paging_init() { 
//   /* your code */
//   //设置映射
//   uint64_t *pgtbl = &_end;
//   uint64_t pa_start = 0x80000000;
//   uint64_t va_start = 0xffffffe000000000;
//   uint64_t UART = 0x10000000;
//   uint64_t size = 0x1000000;
//   uint64_t text_size = 0x2000;
//   uint64_t rodata_size = 0x1000;
//   uint64_t data_size = size - text_size - rodata_size;

//   int perm = 7;
//   create_mapping(pgtbl, va_start, pa_start, size, perm);  //高地址映射
//   create_mapping(pgtbl, pa_start, pa_start, size, perm);  //kernel等值映射
//   create_mapping(pgtbl, UART, UART, size, perm);          //UART等值映射

//   //权限保护
//   //高地址映射
//   create_mapping(pgtbl, text_start+offset, text_start, text_size, 5);  //text XWR = 101
//   create_mapping(pgtbl, rodata_start+offset, rodata_start, rodata_size, 1);  //rodata XWR = 001
//   create_mapping(pgtbl, data_start+offset, data_start, data_size, 3);  //data XWR = 011
//   //kernel等值映射
//   create_mapping(pgtbl, text_start, text_start, text_size, 5);  //text XWR = 101
//   create_mapping(pgtbl, rodata_start, rodata_start, rodata_size, 1);  //rodata XWR = 001
//   create_mapping(pgtbl, data_start, data_start, data_size, 3);  //data XWR = 011
//   //UART等值映射
//   create_mapping(pgtbl, UART, UART, size, 3);  //data XWR = 011        
// }


void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz, int perm)
{
	/*your code*/
    //提取各级虚拟页号
    int VPN_2 = (va >> 30) & 0x1FF;
    int VPN_1 = (va >> 21) & 0x1FF;
    int VPN_0 = (va >> 12) & 0x1FF;

    uint64_t *second_pgtbl; //二级页表的基地址
    if ((pgtbl[VPN_2] & 0x1) == 0)
    {
        page_count++;   //分配新的物理页
        second_pgtbl = (void *)((uint64_t)(&_end) + 0x1000 * page_count); //获取基地址
        for (int i = 0; i < 512; i++)   //初始化
            second_pgtbl[i] = 0;
        pgtbl[VPN_2] |= (((uint64_t)second_pgtbl >> 12) << 10); //存储二级页表的物理基页
        pgtbl[VPN_2] |= 0x1; //对valid位置位
    }
    second_pgtbl = (void *)((pgtbl[VPN_2] >> 10) << 12);

    uint64_t *third_pgtbl;  //三级页表的基地址
    if ((second_pgtbl[VPN_1] & 0x1) == 0)
    {
        page_count++;
        third_pgtbl = (void *)((uint64_t)(&_end) + 0x1000 * page_count); //获取基地址
        for (int i = 0; i < 512; i++)
            third_pgtbl[i] = 0;
        //存储三级页表的物理基页
        second_pgtbl[VPN_1] |= (((uint64_t)third_pgtbl >> 12) << 10); 
        second_pgtbl[VPN_1] |= 0x1;
    }
    third_pgtbl = (void *)((second_pgtbl[VPN_1] >> 10) << 12);

    third_pgtbl[VPN_0] |= (pa >> 12) << 10; //存储实际的物理页
    third_pgtbl[VPN_0] |= 0x1;              //valid置位
    third_pgtbl[VPN_0] |= perm << 1;        //权限置位
    
    /*
    if ((third_pgtbl[VPN_0] & 0x1) == 0)
    {
        third_pgtbl[VPN_0] |= (pa >> 12) << 10; //存储实际的物理页
        third_pgtbl[VPN_0] |= 0x1;              //valid置位
        third_pgtbl[VPN_0] |= perm << 1;        //权限置位
    }
    */
}