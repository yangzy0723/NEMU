/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

typedef uint32_t PTE;

static inline uintptr_t GET_DIR(uintptr_t p)//页目录索引
{
	return (p & (uintptr_t)0xffc00000) >> 22;
}

static inline uintptr_t GET_PAGE(uintptr_t p)//页表索引
{
	return (p & (uintptr_t)0x003ff000) >> 12;
}

static inline uintptr_t GET_OFFSET(uintptr_t p)//页内偏移量
{
	return p & 0x00000fff;
}

static inline uintptr_t GET_BASE_ADDR(PTE p)//得到基地址
{
	return (uintptr_t)p & 0xfffffc00;
}

//此处不能用指针操作了，因为连接到的是本机的linux系统上
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  uintptr_t page_directory_entry = cpu.satp << 12; 
	//printf("基地址%x\n", (int)page_directory_entry);
	paddr_t page_directory_item_entry = page_directory_entry + GET_DIR((uintptr_t)vaddr) * 4;
	//printf("%x 页目录地址\n", page_directory_item_entry);
	PTE page_directory_item = paddr_read(page_directory_item_entry, 4);
	
	//printf("%x nemu 页目录值\n", page_directory_item);
	assert(page_directory_item & 1);//检查valid位

	uintptr_t page_table_entry = GET_BASE_ADDR(page_directory_item);

	paddr_t page_table_item_entry = page_table_entry + GET_PAGE((uintptr_t)vaddr) * 4;

	PTE page_table_item = paddr_read(page_table_item_entry, 4);

	assert(page_table_item & 1);//检查valid位

	paddr_t pa = (paddr_t)((GET_BASE_ADDR(page_table_item) << 2) + GET_OFFSET((uintptr_t)vaddr));
	return pa;
}

int isa_mmu_check(vaddr_t vaddr, int len, int type)
{
	if(((uint32_t)(cpu.satp & 0x80000000)) >> 31 == 1)
		return MMU_TRANSLATE;
	else
		return MMU_DIRECT;
}
