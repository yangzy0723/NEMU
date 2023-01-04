#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = (int)(*args);
	char **argv = (char **)(args + 1);
	char **envp = (char **)(args + 1 + argc + 1);
  environ = envp;
	
	printf("argv参数个数: %d\n",*(args));
	printf("argv的第一个: %s\n",(char *)*(argv));
	printf("envp的第一个: %s\n",(char *)*(envp));
	//printf("%s\n",(char *)*(args + 3));
	//printf("%s\n",(char *)*(args + 4));
	//printf("%s\n",(char *)*(args + 5));
  exit(main(argc, argv, envp));
  assert(0);
}
