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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint32_t choose(uint32_t n)
{
	return rand()%n;
}

uint32_t now_position = 0;

void gen_num()
{
	uint32_t tmp = rand()%1000;
	if(tmp == 0)
	{
		buf[now_position] = '0';
		now_position++;
	}
	else
	{
		uint32_t old_position = now_position;
		if(tmp >= 100)
			now_position += 3;//三位数处理。
		else if(tmp >= 10)
			now_position += 2;//两位数处理。
		else
			now_position += 1;//一位数处理。
		for(int j = 1; j <= now_position - old_position; j++)
		{
			buf[now_position - j] = tmp % 10 + '0';
			tmp = tmp / 10;
		}
	}
}

void gen(char op)
{
	buf[now_position] = op;
	now_position++;
}

void gen_rand_op()
{
	int op = rand()%4;
	if(op == 0)
		gen('+');
	else if(op == 1)
		gen('-');
	else if(op == 2)
		gen('*');
	else if(op == 3)
		gen('/');
}
static void gen_rand_expr() 
{
	switch(choose(3))
	{
		case 0: gen_num(); break;
		case 1: gen('('); gen_rand_expr(); gen(')'); break;
		default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
	}
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  for (int i = 0; i < loop; i ++) 
	{
    gen_rand_expr();
		
		buf[now_position] = 0;//保护作用。

    sprintf(code_buf, code_format, buf);//将code_format中的格式化符号替换为buf，全部读入code_buf中。

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);//将code_buf中的代码放进fp文件中，也就是.code.c文件里。
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");//对.code.c文件进行编译。
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if(fscanf(fp, "%d", &result)){};//将结果从fp（也就是.expr文件）中取出，读入result中，然后打印。
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}


