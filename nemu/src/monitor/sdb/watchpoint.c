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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	word_t original_value;
	char expr[108];//假设这是人能写出来的表达式。 

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp()
{
	if(free_ == NULL)
		printf("no more free watchpoint!\n");
	WP* tmp = free_;
	free_ = free_ -> next;
	tmp -> next = head;
	head = tmp;
	return head;
}

void free_wp(int num)
{
	if(head == &wp_pool[num])
	{
		head = head->next;
		(&wp_pool[num]) -> next = free_;
		free_ = &wp_pool[num];
		return;
	}
	else
	{
		WP* tmp = head;
		while(tmp != NULL)
		{
			if(tmp -> next == &wp_pool[num])
			{
				tmp -> next = tmp -> next -> next;
				(&wp_pool[num]) -> next = free_;
				free_ = &wp_pool[num];
				break;
			}
			tmp = tmp -> next;
		}
	}
}

WP* get_head()
{
	return head;
}

WP* get_wlpool()
{
	return wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

