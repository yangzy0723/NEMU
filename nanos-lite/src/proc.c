#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void(*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
	context_uload(&pcb[0], "/bin/pal", NULL, NULL);
	context_kload(&pcb[1], hello_fun, "yzy & zqy");
  switch_boot_pcb();
	Log("Initializing processes...");
}

Context* schedule(Context *prev) {
	current->cp = prev;
	//current = current == &pcb[1] ? &pcb[0] : &pcb[1];
	current = &pcb[1];
	return current->cp;
}

int execve(char *filename, char *const argv[], char *const envp[])
{
	context_uload(&pcb[1], filename, argv, envp);
	switch_boot_pcb();
	yield();
	return 0;
}
