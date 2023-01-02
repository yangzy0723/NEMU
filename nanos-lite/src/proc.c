#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);

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
	context_kload(&pcb[0], hello_fun, "zqy");
	context_kload(&pcb[1], hello_fun, "yzy");
	context_kload(&pcb[2], hello_fun, "123");
  switch_boot_pcb();

  //Log("Initializing processes...");

  // load program here
	//naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  current->cp = prev;
	if(current == &pcb[0])
		current = &pcb[1];
	else if(current == &pcb[1])
		current = &pcb[2];
	else
		current = &pcb[0];
	if(current == &pcb[0])
		printf("1\n");
	else
		printf("2\n");
	return current->cp;
}
