#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void(*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

static PCB pcb_boot = {};
PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
PCB *current = NULL;
int now_pcb = 1;

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
	context_kload(&pcb[0], hello_fun, "yzy & zqy");
	context_uload(&pcb[1], "/bin/pal", NULL, NULL);
	context_uload(&pcb[2], "/bin/bird", NULL, NULL);
	context_uload(&pcb[3], "/bin/menu", NULL, NULL);
  switch_boot_pcb();
	Log("Initializing processes...");
}

void switch_to_pal()
{
	now_pcb = 1;
}

void switch_to_bird()
{
	now_pcb = 2;
}

void switch_to_menu()
{
	now_pcb = 3;
}
Context* schedule(Context *prev) {
	current->cp = prev;
	current = current == &pcb[0] ? &pcb[now_pcb] : &pcb[0];
	//current = &pcb[0];
	return current->cp;
}

int execve(char *filename, char *const argv[], char *const envp[])
{
	context_uload(&pcb[1], filename, argv, envp);
	switch_boot_pcb();
	yield();
	return 0;
}
