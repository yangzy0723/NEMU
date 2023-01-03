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
	printf("%d\n",*(args));
	printf("%s\n",(char *)*(args + 1));
	printf("%s\n",(char *)*(args + 2));
	printf("%s\n",(char *)*(args + 3));
	printf("%s\n",(char *)*(args + 4));
	printf("%s\n",(char *)*(args + 5));
  exit(main(argc, argv, envp));
  assert(0);
}
