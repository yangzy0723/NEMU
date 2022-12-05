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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#include <stdio.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

#ifdef CONFIG_IRINGBUF
	void Buff_Write(char * log);
#endif

enum {
  TYPE_I, TYPE_U, TYPE_S, TYPE_R, TYPE_B, TYPE_J,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5)|BITS(i, 11, 7); } while(0)
#define immB() do { *imm = ((((SEXT(BITS(i, 31, 31), 1)<<12)|BITS(i, 7, 7)<<11)|BITS(i, 30, 25)<<5)|BITS(i, 11, 8)<<1)|0; } while(0)
#define immJ() do { *imm = ((((SEXT(BITS(i, 31, 31), 1)<<20)|BITS(i, 19, 12)<<12)|BITS(i, 20, 20)<<11)|BITS(i, 30, 21)<<1)|0; } while(0)


static void decode_operand(Decode *s, int *dest, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rd  = BITS(i, 11, 7);
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *dest = rd;
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
		case TYPE_R: src1R();	src2R();				 break;
		case TYPE_B: src1R(); src2R(); immB(); break;
		case TYPE_J:									 immJ(); break;
  }
}

void csrrs_function(word_t csr, word_t src1, word_t dest)
{
	word_t t = 0;
	switch(csr)
	{
		case 0x341:
				t = cpu.mepc;
				cpu.mepc = t | src1;
				break;
		case 0x300:
				t = cpu.mstatus;
				cpu.mstatus = t | src1;
				break;
		case 0x342:
				t = cpu.mcause;
				cpu.mcause = t | src1;
				break;
		case 0x305:
				t = cpu.mtvec;
				cpu.mtvec = t | src1;
				break;
	}
	R(dest) = t;
	return;
}
void csrrw_function(word_t csr, word_t src1, word_t dest)
{
	word_t t = 0;
	switch(csr)
	{
		case 0x341:
			t = cpu.mepc;
			cpu.mepc = src1;
			break;
		case 0x300:
			t = cpu.mstatus;
			cpu.mstatus = src1;
			break;
		case 0x342:
			t = cpu.mcause;
			cpu.mcause = src1;
			break;
		case 0x305:
			t = cpu.mtvec;
			cpu.mtvec = src1;
			break;
	}
	R(dest) = t;
	return;
}

static int decode_exec(Decode *s) {
  int dest = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
	int64_t tmp;
	bool success;
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
	/*U*/
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(dest) = imm);
	INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = imm + s->pc);
	/*J*/
	INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, s->dnpc = imm + s->pc, R(dest) = s->snpc);
	/*B*/
	INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = (src1 == src2) ? imm + s->pc : s->snpc);
	INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = (src1 != src2) ? imm + s->pc : s->snpc);
	INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = ((sword_t)src1 >= (sword_t)src2) ? imm + s->pc : s->snpc);
	INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = ((sword_t)src1 < (sword_t)src2) ? imm + s->pc : s->snpc);
	INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = (src1 < src2) ? imm + s->pc : s->snpc);
	INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = (src1 >= src2) ? imm + s->pc : s->snpc);
	/*I*/
	INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = imm + src1);
	INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc = (imm + src1)&(-2), R(dest) = s->snpc);
	INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(dest) = src1 < (word_t)imm);
	INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(dest) = (sword_t)src1 >> BITS(imm, 5, 0));
	INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = imm & src1);
	INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(dest) = imm ^ src1);
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = Mr(src1 + imm, 4));
	INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(dest) = SEXT(BITS(Mr(src1 + imm, 4), 15, 0), 16));
	INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(dest) = SEXT(BITS(Mr(src1 + imm, 4), 7, 0), 8));
	INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(dest) = 0x0000ffff&Mr(src1 + imm, 4));
	INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = 0x000000ff&Mr(src1 + imm, 4));
	INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(dest) = src1 << imm);//因为imm[11:5]为0000000,所以可以大胆移动imm位。
	INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(dest) = src1 >> imm);
	INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(dest) = imm | src1);
	INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(dest) = ((sword_t)src1 < (sword_t)imm));
	/*S 一开始写错了！！！ 坑！！！*/
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
	INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
	INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
	/*R*/
	INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(dest) = src1 + src2);
	INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(dest) = src1 - src2);
	INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(dest) = src1 < src2);
	INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(dest) = src1 ^ src2);
	INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(dest) = src1 | src2);
	INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(dest) = src1 & src2);
	INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(dest) = (sword_t)src1 / (sword_t)src2);
	INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(dest) = src1 / src2);
	INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(dest) = (sword_t)src1 % (sword_t)src2);
	INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(dest) = src1 % src2);
	INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(dest) = (sword_t)src1 < (sword_t)src2);
	INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(dest) = (sword_t)src1 * (sword_t)src2);
	INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, tmp = (int64_t)((sword_t)src1)*(int64_t)((sword_t)src2), R(dest) = BITS(tmp, 63, 32));
	//要先转成有符号数字相乘，因为是有符号乘。
	INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, tmp = (uint64_t)src1*(uint64_t)src2, R(dest) = BITS(tmp, 63, 32));
	INSTPAT("0000001 ????? ????? 010 ????? 01100 11", mulhsu , R, tmp = (int64_t)((sword_t)src1)*(uint64_t)src2, R(dest) = BITS(tmp, 63, 32));
	INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(dest) = (sword_t)src1 >> BITS(src2, 4, 0));
	INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(dest) = src1 >> BITS(src2, 4, 0));
	INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(dest) = src1 << BITS(src2, 4, 0));
	/*N*/
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0			
	/*privileged instruction*/
	INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I, csrrs_function(imm, src1, dest));
	INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, csrrw_function(imm, src1, dest));	
	INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, s->dnpc = isa_raise_intr(isa_reg_str2val("a7", &success), s->pc));
	INSTPAT("0011000 00010 00000 000 00000 11100 11", mret   , N, s->dnpc = cpu.mepc + 4);
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);

/*自己加的*/
#ifdef CONFIG_IRINGBUF
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
	Buff_Write(s->logbuf);
#endif

  return decode_exec(s);
}
