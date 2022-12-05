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
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
#ifdef CONFIG_ETRACE
	printf("yzy\n");
	log_write("\n\nAt PC_ADDRESS " FMT_WORD ", an error is triggered.\nThe error_num(cpu.mcause) is" FMT_WORD "\n\n\n",epc, NO);
#endif  
	cpu.mepc = epc;
	cpu.mcause = NO;
	return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
