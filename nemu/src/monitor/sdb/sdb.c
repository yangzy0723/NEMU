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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	word_t original_value;
	char expr[108]; 
} WP;

void init_regex();
void init_wp_pool();

WP* new_wp();
WP* get_head();
void free_wp(int num);
word_t vaddr_read(word_t addr, int len);//声明一下，不然会报错。

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args);

static int cmd_q(char *args);

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "si [N]:Let the progam execute N instructions and then suspend the execution. When N is not given, it defaults to 1", cmd_si },
	{ "info", "info r: print register status; info w: print watch point information", cmd_info },
	{ "x", "x N EXPR: Figure out the value of EXPR, take the result as the starting memory address, and output N consecutive 4 bytes in hexadecimal form", cmd_x },
	{ "p", "p EXPR: expression evaluation", cmd_p},
	{ "w", "w EXPR: Pause the program when the value of expression EXPR changes", cmd_w},
	{ "d", "d N: Delete the watchpoint N", cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

/* cmd_c */
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

/* cmd_q */
static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;//更改nemu_state状态，安全退出，见utlis/state.c
  return -1;
}

/* cmd_help */
static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

/* cmd_si */
static int cmd_si(char *args)
{
	if(args == NULL)
		cpu_exec(1);	

	else
	{
		char* record = args;
		int length = strlen(args);
		word_t num = 0;
		bool if_leagl_cmd = true;
		for(int i = 0; i < length; i++, args++)
		{
			if('0' <= *args && *args <= '9')
				num = num * 10 + (*args - '0');
			else
			{	
				if_leagl_cmd = false;
				break;
			}//若传入无效参数，空格，字母啥的，直接返回Unknown command。
		}
		if(if_leagl_cmd)
			cpu_exec(num);
		else
			printf("Unknown command '%s'\n", record);
	}
	return 0;
}

/* cmd_info */
static int cmd_info(char *args)
{
	if(args == NULL)
	{
		printf("Unknown command\n");//此处未传入参数，直接视为无效指令。
		return 0;
	}
	if(strlen(args) == 1)
	{
		if(*args == 'r') isa_reg_display();
		else if(*args == 'w')
		{
			printf("NUM\tEXPR\tORIGINAL_VALUE\n");
			WP* head = get_head();
			while(head != NULL)
			{
				printf("%d\t%s\t%#010x\n",head->NO, head->expr, head->original_value);
				head = head -> next;
			}
		}
		else	printf("Unknown command '%s'\n", args);
	}
	else
		printf("Unknown command '%s'\n", args);
	return 0;
}

/* cmd_x */
static int cmd_x(char *args)
{
	if(args == NULL)
	{
		printf("Unknown command\n");//此处未传入参数，直接视为无效指令。
		return 0;
	}

	char* arg = strtok(args, " ");	
	if(arg == NULL)
	{
		printf("Unknown command\n");//无参数，直接视为无效指令。
		return 0;
	}
	else
	{
		int num = 0;
		for(int i = 0; *(arg + i) != 0; i++)
			if(*(arg + i) >= '0' && *(arg + i) <= '9')
				num = num * 10 + *(arg + i) - '0';
			else
			{
				printf("Unknown command '%s'\n", arg);
				return 0;
			}//第一个数字命令出现错误，打印。

		bool success = true; 
		word_t where = expr(args + strlen(args) + 1, &success);

		if(success)
			for(int i = 0; i < num; i++)
				printf("%#x\t%#010x\n", where+4*i, vaddr_read(where+i*4, 4));
		else
			printf("illegal expression %s\n", args+strlen(args)+1);
	}
	return 0;
}

/* cmd_p */
static int cmd_p(char* args)
{
	if(args == NULL)
	{
		printf("Unknown command\n");
		return 0;
	}
	else
	{
		bool success = true;
		word_t ans = expr(args, &success);
		if(success)
			printf("%u\n", ans);
		else
			printf("illegal expression %s\n", args);
		return 0;
	}
}

/* cmd_w */
static int cmd_w(char* args)
{
	if(args == NULL)
	{
		printf("Unknown command\n");
		return 0;
	}
	bool success = true;
	word_t record = expr(args, &success);
	if(success)
	{
		WP* new_point = new_wp();
		if(new_point != NULL)
		{
			strcpy(new_point -> expr, args);
			new_point -> original_value = record; 
		}
	}
	else
		printf("illegal expression %s\n", args);
	return 0;
}

/* cmd_d */
static int cmd_d(char* args)
{
	if(args == NULL)
		printf("Unknown command\n");
	else
	{
		int num = 0;
		char* tmp = args;
		while(*tmp != 0)
		{
			if(*tmp <= '9' && *tmp >= '0')
			{
				num = num*10 + *args - '0';
				tmp++;
			}
			else
			{
				printf("Unknown command %s\n", args);
				return 0;
			}
		}
		if(num >= 0 && num < 31)
			free_wp(num);	
		else
			printf("Watchpoint %d does not exist!\n", num);
	}
	return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
