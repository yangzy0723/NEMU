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
#include <stdio.h>

#define IRQ_TIMER 0x80000007;

word_t isa_raise_intr(word_t NO, vaddr_t epc) {

#ifdef CONFIG_ETRACE
	printf("\nAt PC_ADDRESS " FMT_WORD ", an error is triggered. The error_num(cpu.mcause) is " FMT_WORD "\n\n", epc, NO);
#endif

	cpu.mepc = epc;
	cpu.mcause = NO;
	if((cpu.mstatus & 0x8) >> 3)
		cpu.mstatus = cpu.mstatus | 0x80;
	else
		cpu.mstatus = cpu.mstatus & (~0x80);
	cpu.mstatus = cpu.mstatus & (~0x8);//将MIE保存到MPIE中，将MIE置为0
	return cpu.mtvec;
}

word_t isa_query_intr() {
	if((cpu.mstatus & 0x80) >> 7 && cpu.INTR)//MPIE所在的位置
	{
		cpu.INTR = false;
		return IRQ_TIMER;
	}
	return INTR_EMPTY;
}
