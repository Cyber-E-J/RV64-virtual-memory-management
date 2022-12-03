#include"vm.h"
#include"put.h"
  
extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;
#define PAGE_SIZE  4096UL

int page_count = 1;

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

void paging_init()
{
    
    // uint64_t *pgtbl = &_end;
    // //对一级页表项进行初始化
    // for (int i = 0; i < 512; i++)
    // {
    //     pgtbl[i] = 0;
    // }

    // for (uint64_t va = 0xffffffe000000000; va < 0xffffffe001000000; va = va + 0x1000)
    // {
    //     create_mapping(pgtbl, va, va - offset, 0x1000, 7);
    //     create_mapping(pgtbl, va - offset, va - offset, 0x1000, 7);
    // }

    // for (uint64_t pa = 0x10000000; pa < 0x10001000; pa = pa + PAGE_SIZE)
    // {
    //     create_mapping(pgtbl, pa, pa, 0x1000, 7);
    // }
    
    uint64_t *pgtbl = &_end;
    //对一级页表项进行初始化
    for (int i = 0; i < 512; i++)
    {
        pgtbl[i] = 0;
    }

    //对text段的映射，包含高地址映射和等值映射
    for (uint64_t pa = 0x80000000; pa < 0x80000000 + (uint64_t)(&rodata_start) - (uint64_t)(&text_start); pa += PAGE_SIZE)
    {
        create_mapping(pgtbl, pa + offset, pa, 0x1000, 5);
        create_mapping(pgtbl, pa, pa, 0x1000, 5);
    }

    //对rodata段的映射，包含高地址映射和等值映射
    for (uint64_t pa = 0x80000000 + (uint64_t)(&rodata_start) - (uint64_t)(&text_start); pa < 0x80000000 + (uint64_t)(&data_start) - (uint64_t)&(text_start); pa += PAGE_SIZE)
    {
        create_mapping(pgtbl, pa + offset, pa, 0x1000, 1);
        create_mapping(pgtbl, pa, pa, 0x1000, 1);
    }

    //对其他段的映射，包含高地址映射和等值映射
    for (uint64_t pa = 0x80000000 + (uint64_t)(&data_start)- (uint64_t)(&text_start); pa < 0x81000000; pa += PAGE_SIZE)
    {
        create_mapping(pgtbl, pa + offset, pa, 0x1000, 3);
        create_mapping(pgtbl, pa, pa, 0x1000, 3);
    }

    //对硬件地址的映射
    for (uint64_t pa = 0x10000000; pa < 0x10001000; pa += PAGE_SIZE)
    {
        create_mapping(pgtbl, pa, pa, 0x1000, 3);
    }
}
