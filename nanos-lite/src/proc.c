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
	context_kload(&pcb[0], hello_fun, "yzy & zqy");
	context_uload(&pcb[1], "/bin/bird", NULL, NULL);
	context_uload(&pcb[2], "/bin/pal", NULL, NULL);
	context_uload(&pcb[3], "/bin/menu", NULL, NULL);
  switch_boot_pcb();
	Log("Initializing processes...");
}

int which_app = 1;
void switch_to_pal()
{
	which_app = 1;
}

void switch_to_bird()
{
	which_app = 2;
}

void switch_to_menu()
{
	which_app = 3;
}
Context* schedule(Context *prev) {
	current->cp = prev;
	current = current == &pcb[0] ? &pcb[which_app] : &pcb[0];
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
