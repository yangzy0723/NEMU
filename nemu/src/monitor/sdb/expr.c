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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
	{"-", '-'},						// subtraction
	{"\\*", '*'},					// mul
	{"/", '/'},						// divide
	{"\\)", ')'},					// right parentheses
	{"\\(", '('},         // left parentheses
	{"[0-9]+",TK_NUM},    // number
};

word_t eval(bool* success, int p, int q);

bool check_parentheses(int p, int q);

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};//用于存储编译好的正则表达式。

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];//存放错误信息的字符串
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);//ret是错误编号，err_msg用于储存regerror（）返回的错误信息。
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;//Token是数据类型

static int tokens_size = 0;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e)//e是待解析的目标字符串。 
{
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;
  while (e[position] != '\0') 
	{
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) 
		{
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) 
			{
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type)
				{
					case TK_NOTYPE:;break;
					case '+':{tokens[nr_token].type = '+'; nr_token++;}; break;
					case TK_EQ:{tokens[nr_token].type = TK_EQ; nr_token++;}; break;
					case '-':{tokens[nr_token].type = '-'; nr_token++;}; break;
					case '*':{tokens[nr_token].type = '*'; nr_token++;}; break;
					case '/':{tokens[nr_token].type = '/'; nr_token++;}; break;
					case '(':{tokens[nr_token].type = '('; nr_token++;}; break;
					case ')':{tokens[nr_token].type = ')'; nr_token++;}; break;
					case TK_NUM:
									{
										tokens[nr_token].type = TK_NUM;
										for(int j = 0; j < substr_len; j++)
											tokens[nr_token].str[j] = substr_start[j]; //将123存入str数组的模式为1 2 3 \0。
										tokens[nr_token].str[substr_len] = 0;//加一个终止符，保证前面的运算结果存储不会影响到后面的。
										nr_token++;
									}; break;		
          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) 
		{
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  tokens_size = nr_token;
  return true;
}


word_t expr(char *e, bool *success) 
{
  if (!make_token(e)) 
	{
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */

	//printf("%d\n",tokens_size);
	return eval(success, 0, tokens_size - 1);//tokens_size记录tokens数组哪些位数有效，从而确定p，q。
}

bool check_parentheses(int p, int q)
{
	int top = 0;
	for(int i = p+1; i <= q-1; i++)
			if(tokens[i].type == '(')
				top++;
			else if(tokens[i].type == ')')
			{
				top--;
				if(top < 0)
					return false;
			}
	if(top != 0)
		return false;
	if(tokens[p].type == '(' && tokens[q].type == ')')
		return true;
	return false;
}

word_t eval(bool* success, int p, int q)
{
	if(p > q)
	{
		*success = false;
		return 0;
	}
	else if(p == q)
	{
		if(tokens[p].type == TK_NUM)
		{
			int i = 0;
			word_t result = 0;
			while(tokens[p].str[i] != 0)
			{
				result = result * 10 + tokens[p].str[i] - '0';
				i++;
			}
			return result;
		}
		else
		{
			*success = false;
			return 0;
		}
	}
	else if(check_parentheses(p, q))
		return eval(success, p + 1, q - 1);
	else
	{
		int top = 0;
		int op = 0;
		int if_plus_sub = 0;
		for(int i = p; i <= q; i++)
		{
			if((tokens[i].type == '+' || tokens[i].type == '-') && top == 0)
			{
				op = i;
				if_plus_sub = 1;
			}
			else if((tokens[i].type == '*' || tokens[i].type == '/') && if_plus_sub == 0 && top == 0)
				op = i;
			else if(tokens[i].type == '(')
				top++;
			else if(tokens[i].type == ')')
				top--;
		}
		if(top != 0)
		{
			*success = false;
			return 0;
		}//括号不匹配，返回非法值。
		int val1 = eval(success, p, op - 1);
		int val2 = eval(success, op + 1, q);
		switch(tokens[op].type)
		{
			case '+': return val1+val2;
			case '-': return val1-val2;
			case '*': return val1*val2;
			case '/': 
				{
					if(val2 == 0)
					{
						printf("Denominator is 0\n");
						*success = false;
						return 0;
					}
					else
						return val1/val2;
				};
			default: {*success = false; return 0;};
		}
	}
}
